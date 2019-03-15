#include <iostream>

#include "FormatVisitor.h"
#include "LuaLexer.h"

using namespace std;
using namespace antlr4;

#define LOG_FLAG

#ifdef LOG_FLAG
#define LOGVAR(arg) cout << "" #arg " = " << (arg) << endl
#define LOG(arg) cout << arg << endl
#else
#define LOG(arg) ;
#define LOGVAR(arg) ;
#endif

static bool isBlankChar(char c) { return c == ' ' || c == '\t' || c == '\u000C'; }

static bool isLastWhiteSpace(stringstream& ss) {
    if (!ss.tellp()) {
        return false;
    }
    ss.seekg(-1, ios::end);
    return ss.peek() == ' ';
}

int lastNodeIndex(tree::ParseTree* t) {
    if (t == NULL) {
        return -1;
    }
    ParserRuleContext* c = dynamic_cast<ParserRuleContext*>(t);
    if (c != NULL) {
        return c->getStop()->getTokenIndex();
    }
    tree::TerminalNode* n = dynamic_cast<tree::TerminalNode*>(t);
    if (n != NULL) {
        return n->getSymbol()->getTokenIndex();
    }
    return INT_MAX;
}

// commentAfter returns comments between a ParseTreeNode and the next Node.
// expect: return expect string if no comments found
string FormatVisitor::commentAfter(tree::ParseTree* node, const string& expect) {
    int l = lastNodeIndex(node);
    stringstream ss;
    // No space on left of first comment
    bool lastComment = node != NULL;
    for (int i = l + 1; i < tokens_.size(); i++) {
        auto token = tokens_[i];
        if (token->getType() == LuaLexer::COMMENT) {
            if (lastComment) {
                ss << " ";
            }
            ss << token->getText();
            lastComment = true;
        } else if (token->getType() == LuaLexer::LINE_COMMENT) {
            if (lastComment) {
                ss << " ";
            }
            ss << formatLineComment(token);
            ss << indent();
            lastComment = false;
        } else if (token->getType() == LuaLexer::SHEBANG) {
            ss << token->getText() << "\n\n";
        } else if (token->getType() == LuaLexer::WS) {
        } else {
            break;
        }
    }
    if (!ss.tellp()) {
        ss << expect;
    } else if (lastComment) {
        ss << " ";
    }
    return ss.str();
}

// commentAfterNewLine returns comments between a ParseTreeNode and the next Node.
// This function always expect a line break.
// intdentSize: inc or dec indent after a line break.
string FormatVisitor::commentAfterNewLine(tree::ParseTree* node, int indentSize) {
    ParserRuleContext* ctx = dynamic_cast<ParserRuleContext*>(node);
    if (ctx != NULL && ctx->children.size() == 0) {
        indent_ += indentSize;
        return "";
    }
    int l = lastNodeIndex(node);
    if (indentSize > 0) {
        indent_ += indentSize;
    }
    stringstream ss;
    bool customNewLine = false;
    bool lastComment = true;
    bool lastestNewLine = false;
    for (int i = l + 1; i < tokens_.size(); i++) {
        auto token = tokens_[i];
        if (token->getType() == LuaLexer::COMMENT) {
            if (lastestNewLine) {
                ss << indent();
            } else if (lastComment) {
                ss << " ";
            }
            ss << token->getText();
            lastComment = true;
            lastestNewLine = false;
        } else if (token->getType() == LuaLexer::LINE_COMMENT) {
            if (lastestNewLine) {
                ss << indent();
            } else if (lastComment) {
                ss << " ";
            }
            ss << formatLineComment(token);
            customNewLine = true;
            lastComment = false;
            lastestNewLine = true;
        } else if (token->getType() == LuaLexer::WS) {
            int ln = token->getText().find("\n");
            int rn = token->getText().rfind("\n");
            if (ln != string::npos) {
                if (ln != rn && !lastestNewLine) {
                    ss << "\n\n";
                } else {
                    ss << "\n";
                }
                customNewLine = true;
                lastComment = false;
                lastestNewLine = true;
            }
        } else {
            break;
        }
    }
    if (!customNewLine) {
        ss << "\n";
        lastestNewLine = true;
    }
    if (indentSize < 0) {
        indent_ += indentSize;
    }
    if (lastComment && !lastestNewLine) {
        ss << "\n";
    }
    return ss.str();
}

void FormatVisitor::pushWriter() { writers_.push_back(new SourceWriter()); }

string FormatVisitor::popWriter() {
    SourceWriter* writer = writers_.back();
    string ret = writer->str();
    writers_.pop_back();
    delete writer;
    return ret;
}

SourceWriter& FormatVisitor::cur_writer() { return *writers_.back(); }

int FormatVisitor::cur_columns() { return (*writers_.back()).columns(); }

// Add a white space before line comment
string FormatVisitor::formatLineComment(Token* token) {
    string comment = token->getText();
    char firstChar = comment[2];
    if (isBlankChar(firstChar) || firstChar == '-' || firstChar == '\r' || firstChar == '\n') {
        // do nothing
    } else {
        comment = "-- " + comment.substr(2, comment.size());
    }
    return comment;
}

antlrcpp::Any FormatVisitor::visitChunk(LuaParser::ChunkContext* ctx) {
    LOG("visitChunk");
    pushWriter();
    cur_writer() << commentAfter(NULL, "");
    bool temp = chop_down_block_;
    chop_down_block_ = true;
    visitBlock(ctx->block());
    chop_down_block_ = temp;
    string comment = commentAfterNewLine(ctx->block(), 0);

    cur_writer() << comment;
    // If there is no line break at the end of the file, add one
    for (int i = comment.size() - 1; i >= 0; i--) {
        if (isBlankChar(comment[i])) {
            continue;
        }
        if (comment[i] != '\n') {
            cur_writer() << "\n";
        }
        break;
    }
    return popWriter();
}

// stat* retstat?
antlrcpp::Any FormatVisitor::visitBlock(LuaParser::BlockContext* ctx) {
    LOG("visitBlock");
    auto stats = ctx->stat();
    int n = stats.size();
    for (int i = 0; i < n; i++) {
        bool isSemi = stats[i]->SEMI() != NULL;
        bool needComment = i < n - 1 || ctx->retstat() != NULL;
        bool isNextSemi = i + 1 < n && stats[i + 1]->SEMI() != NULL;
        if (!isSemi) {
            visitStat(stats[i]);
        }
        bool previousWhiteSpace = isSemi || isLastWhiteSpace(cur_writer().ss());
        if (needComment) {
            if (isNextSemi) {
                string comment = commentAfter(stats[i], "");
                if (previousWhiteSpace && comment.size() > 0 && comment[0] == ' ') {
                    cur_writer() << comment.substr(1, comment.size() - 1);
                } else {
                    cur_writer() << comment;
                }
            } else {
                string comment = commentAfterNewLine(stats[i], 0);
                if (previousWhiteSpace && comment.size() > 0 && comment[0] == ' ') {
                    cur_writer() << comment.substr(1, comment.size() - 1);
                } else {
                    cur_writer() << comment;
                }
            }
        }
    }
    if (ctx->retstat() != NULL) {
        visitRetstat(ctx->retstat());
    }
    if (ctx->retstat() == NULL && isLastWhiteSpace(cur_writer().ss())) {
        string blockComment = commentAfter(ctx, "");
        if (blockComment.size() > 0 && blockComment[0] == ' ') {
            LOG("seekp");
            cur_writer().ss().seekp(-1, ios::end);
        }
    }
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitStat(LuaParser::StatContext* ctx) {
    LOG("visitStat");
    if (chop_down_block_) {
        cur_writer() << indent();
    }
    visitChildren(ctx);
    return nullptr;
}

// varlist EQL explist;
antlrcpp::Any FormatVisitor::visitVarDecl(LuaParser::VarDeclContext* ctx) {
    LOG("visitVarDecl");
    visitVarlist(ctx->varlist());
    cur_writer() << commentAfter(ctx->varlist(), " ");
    cur_writer() << ctx->EQL()->getText();
    cur_writer() << commentAfter(ctx->EQL(), " ");
    bool temp = chop_down_parameter_;
    chop_down_parameter_ = false;
    visitExplist(ctx->explist());
    chop_down_parameter_ = temp;
    if (ctx->SEMI() != NULL) {
        cur_writer() << commentAfter(ctx->explist(), "");
        if (shouldKeepSemicolon(ctx->explist(), ctx->SEMI())) {
            cur_writer() << ctx->SEMI()->getText();
        }
    }
    return nullptr;
}

// varOrExp nameAndArgs+;
antlrcpp::Any FormatVisitor::visitFunctioncall(LuaParser::FunctioncallContext* ctx) {
    LOG("visitFunctioncall");
    visitVarOrExp(ctx->varOrExp());

    bool needWhiteSpace = false;
    if (ctx->nameAndArgs().size() > 0) {
        LuaParser::NameAndArgsContext* naaCtx = ctx->nameAndArgs().front();
        if (naaCtx->COLON() == NULL) {
            LuaParser::StringContext* stringContext = naaCtx->args()->string();
            needWhiteSpace = stringContext != NULL;
        }
    }
    cur_writer() << commentAfter(ctx->varOrExp(), needWhiteSpace ? " " : "");

    int n = ctx->nameAndArgs().size();
    for (int i = 0; i < n; i++) {
        visitNameAndArgs(ctx->nameAndArgs()[i]);
        if (i != n - 1) {
            needWhiteSpace = false;
            LuaParser::NameAndArgsContext* naaCtx = ctx->nameAndArgs()[i + 1];
            if (naaCtx->COLON() == NULL) {
                LuaParser::StringContext* stringContext = naaCtx->args()->string();
                needWhiteSpace = stringContext != NULL;
            }
            cur_writer() << commentAfter(ctx->nameAndArgs()[i], needWhiteSpace ? " " : "");
        }
    }
    if (ctx->SEMI() != NULL) {
        cur_writer() << commentAfter(ctx->nameAndArgs().back(), "");
        if (shouldKeepSemicolon(ctx->nameAndArgs().back(), ctx->SEMI())) {
            cur_writer() << ctx->SEMI()->getText();
        }
    }
    return nullptr;
}

// GOTO NAME;
antlrcpp::Any FormatVisitor::visitGotoStat(LuaParser::GotoStatContext* ctx) {
    LOG("visitGotoStat");
    cur_writer() << ctx->GOTO()->getText();
    cur_writer() << commentAfter(ctx->GOTO(), " ");
    cur_writer() << ctx->NAME()->getText();
    return nullptr;
}

// DO block END;
antlrcpp::Any FormatVisitor::visitDoStat(LuaParser::DoStatContext* ctx) {
    LOG("visitDoStat");
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block());
    cur_writer() << ctx->END()->getText();
    return nullptr;
}

// WHILE exp DO block END;
antlrcpp::Any FormatVisitor::visitWhileStat(LuaParser::WhileStatContext* ctx) {
    LOG("visitWhileStat");
    cur_writer() << ctx->WHILE()->getText();
    cur_writer() << commentAfter(ctx->WHILE(), " ");
    visitExp(ctx->exp());
    cur_writer() << commentAfter(ctx->exp(), " ");
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block());
    cur_writer() << ctx->END()->getText();
    return nullptr;
}

// REPEAT block UNTIL exp;
antlrcpp::Any FormatVisitor::visitRepeatStat(LuaParser::RepeatStatContext* ctx) {
    LOG("visitRepeatStat");
    cur_writer() << ctx->REPEAT()->getText();
    visitBlockAndComment(ctx->REPEAT(), ctx->block());
    cur_writer() << ctx->UNTIL()->getText();
    cur_writer() << commentAfter(ctx->UNTIL(), " ");
    visitExp(ctx->exp());
    if (ctx->SEMI() != NULL) {
        cur_writer() << commentAfter(ctx->exp(), "");
        if (shouldKeepSemicolon(ctx->exp(), ctx->SEMI())) {
            cur_writer() << ctx->SEMI()->getText();
        }
    }
    return nullptr;
}

// IF exp THEN block (ELSEIF exp THEN block)* (ELSE block)? END;
antlrcpp::Any FormatVisitor::visitIfStat(LuaParser::IfStatContext* ctx) {
    LOG("visitIfStat");
    // keep if statement one line only when there is no elseif or else statement.
    cur_writer() << ctx->IF()->getText();
    cur_writer() << commentAfter(ctx->IF(), " ");
    visitExp(ctx->exp().front());
    cur_writer() << commentAfter(ctx->exp().front(), " ");
    cur_writer() << ctx->THEN().front()->getText();
    if (ctx->ELSEIF().size() == 0 && ctx->ELSE() == NULL) {
        if (needKeepBlockOneLine(ctx->THEN().front(), ctx->block().front())) {
            cur_writer() << commentAfter(ctx->THEN().front(), " ");
            bool temp = chop_down_block_;
            chop_down_block_ = false;
            visitBlock(ctx->block().front());
            chop_down_block_ = temp;
            if (!isBlockEmpty(ctx->block().front())) {
                cur_writer() << commentAfter(ctx->block().front(), " ");
            }
            cur_writer() << ctx->END()->getText();
            return nullptr;
        }
    }
    cur_writer() << commentAfterNewLine(ctx->THEN().front(), 1);
    visitBlock(ctx->block().front());
    cur_writer() << commentAfterNewLine(ctx->block().front(), -1);
    int n = ctx->ELSEIF().size();
    for (int i = 0; i < n; i++) {
        cur_writer() << indent();
        cur_writer() << ctx->ELSEIF()[i]->getText();
        cur_writer() << commentAfter(ctx->ELSEIF()[i], " ");
        visitExp(ctx->exp()[i + 1]);
        cur_writer() << commentAfter(ctx->exp()[i + 1], " ");
        cur_writer() << ctx->THEN()[i + 1]->getText();
        cur_writer() << commentAfterNewLine(ctx->THEN()[i + 1], 1);
        visitBlock(ctx->block()[i + 1]);
        cur_writer() << commentAfterNewLine(ctx->block()[i + 1], -1);
    }
    if (ctx->ELSE() != NULL) {
        cur_writer() << indent();
        cur_writer() << ctx->ELSE()->getText();
        cur_writer() << commentAfterNewLine(ctx->ELSE(), 1);
        visitBlock(ctx->block().back());
        cur_writer() << commentAfterNewLine(ctx->block().back(), -1);
    }
    cur_writer() << indent();  //
    cur_writer() << ctx->END()->getText();
    return nullptr;
}

// FOR NAME EQL exp COMMA exp (COMMA exp)? DO block END;
antlrcpp::Any FormatVisitor::visitForStat(LuaParser::ForStatContext* ctx) {
    LOG("visitForStat");
    cur_writer() << ctx->FOR()->getText();
    cur_writer() << commentAfter(ctx->FOR(), " ");
    cur_writer() << ctx->NAME()->getText();
    cur_writer() << commentAfter(ctx->NAME(), " ");
    cur_writer() << ctx->EQL()->getText();
    cur_writer() << commentAfter(ctx->EQL(), " ");
    visitExp(ctx->exp()[0]);
    cur_writer() << commentAfter(ctx->exp()[0], "");
    cur_writer() << ctx->COMMA()[0]->getText();
    cur_writer() << commentAfter(ctx->COMMA()[0], " ");
    visitExp(ctx->exp()[1]);
    if (ctx->COMMA().size() > 1) {
        cur_writer() << commentAfter(ctx->exp()[1], "");
        cur_writer() << ctx->COMMA()[1]->getText();
        cur_writer() << commentAfter(ctx->COMMA()[1], " ");
        visitExp(ctx->exp()[2]);
        cur_writer() << commentAfter(ctx->exp()[2], " ");
    } else {
        cur_writer() << commentAfter(ctx->exp()[1], " ");
    }
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block());
    cur_writer() << ctx->END()->getText();

    return nullptr;
}

// FOR namelist IN explist DO block END;
antlrcpp::Any FormatVisitor::visitForInStat(LuaParser::ForInStatContext* ctx) {
    LOG("visitForInStat");
    cur_writer() << ctx->FOR()->getText();
    cur_writer() << commentAfter(ctx->FOR(), " ");
    visitNamelist(ctx->namelist());
    cur_writer() << commentAfter(ctx->namelist(), " ");
    cur_writer() << ctx->IN()->getText();
    cur_writer() << commentAfter(ctx->IN(), " ");
    bool temp = chop_down_parameter_;
    chop_down_parameter_ = false;
    visitExplist(ctx->explist());
    chop_down_parameter_ = temp;
    cur_writer() << commentAfter(ctx->explist(), " ");
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block());
    cur_writer() << ctx->END()->getText();
    return nullptr;
}

// FUNCTION funcname funcbody;
antlrcpp::Any FormatVisitor::visitFuncStat(LuaParser::FuncStatContext* ctx) {
    LOG("visitFuncStat");
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), " ");
    visitFuncname(ctx->funcname());
    cur_writer() << commentAfter(ctx->funcname(), "");
    visitFuncbody(ctx->funcbody());
    return nullptr;
}

// LOCAL FUNCTION NAME funcbody;
antlrcpp::Any FormatVisitor::visitLocalFuncStat(LuaParser::LocalFuncStatContext* ctx) {
    LOG("visitLocalFuncStat");
    cur_writer() << ctx->LOCAL()->getText();
    cur_writer() << commentAfter(ctx->LOCAL(), " ");
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), " ");
    cur_writer() << ctx->NAME()->getText();
    cur_writer() << commentAfter(ctx->NAME(), "");
    visitFuncbody(ctx->funcbody());
    return nullptr;
}

// LOCAL namelist (EQL explist)? SEMI?;
antlrcpp::Any FormatVisitor::visitLocalVarDecl(LuaParser::LocalVarDeclContext* ctx) {
    LOG("visitLocalVarDecl");
    cur_writer() << ctx->LOCAL()->getText();
    cur_writer() << commentAfter(ctx->LOCAL(), " ");
    visitNamelist(ctx->namelist());
    if (ctx->EQL() != NULL) {
        cur_writer() << commentAfter(ctx->namelist(), " ");
        cur_writer() << ctx->EQL()->getText();
        cur_writer() << commentAfter(ctx->EQL(), " ");
        bool temp = chop_down_parameter_;
        chop_down_parameter_ = false;
        visitExplist(ctx->explist());
        chop_down_parameter_ = temp;
    }
    if (ctx->SEMI() != NULL) {
        if (ctx->EQL() == NULL) {
            cur_writer() << commentAfter(ctx->namelist(), "");
        } else {
            cur_writer() << commentAfter(ctx->explist(), "");
        }
        if (shouldKeepSemicolon(ctx->explist(), ctx->SEMI())) {
            cur_writer() << ctx->SEMI()->getText();
        }
    }
    return nullptr;
}

// RETURN explist? SEMI?;
antlrcpp::Any FormatVisitor::visitRetstat(LuaParser::RetstatContext* ctx) {
    LOG("visitRetstat");
    if (chop_down_block_) {
        cur_writer() << indent();
    }
    cur_writer() << ctx->RETURN()->getText();
    if (ctx->explist() != NULL) {
        cur_writer() << commentAfter(ctx->RETURN(), " ");
        bool temp = chop_down_parameter_;
        chop_down_parameter_ = false;
        visitExplist(ctx->explist());
        chop_down_parameter_ = temp;
    }
    return nullptr;
}

// var (COMMA var)*;
antlrcpp::Any FormatVisitor::visitVarlist(LuaParser::VarlistContext* ctx) {
    LOG("visitVarlist");
    visitVar(ctx->var().front());
    int n = ctx->COMMA().size();
    if (n > 0) {
        cur_writer() << commentAfter(ctx->var().front(), "");
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        cur_writer() << commentAfter(ctx->COMMA()[i], " ");
        visitVar(ctx->var()[i + 1]);
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->var()[i + 1], "");
        }
    }
    return nullptr;
}

// NAME (COMMA NAME)*;
antlrcpp::Any FormatVisitor::visitNamelist(LuaParser::NamelistContext* ctx) {
    LOG("visitNamelist");
    cur_writer() << ctx->NAME().front()->getText();
    int n = ctx->COMMA().size();
    if (n > 0) {
        cur_writer() << commentAfter(ctx->NAME().front(), "");
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        cur_writer() << commentAfter(ctx->COMMA()[i], " ");
        cur_writer() << ctx->NAME()[i + 1]->getText();
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->NAME()[i + 1], "");
        }
    }
    return nullptr;
}

// exp (COMMA exp)*;
antlrcpp::Any FormatVisitor::visitExplist(LuaParser::ExplistContext* ctx) {
    LOG("visitExplist");
    visitExp(ctx->exp().front());
    int n = ctx->COMMA().size();
    if (n > 0) {
        cur_writer() << commentAfter(ctx->exp().front(), "");
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        if (chop_down_parameter_) {
            cur_writer() << commentAfterNewLine(ctx->COMMA()[i], 0);
            cur_writer() << indent();
        } else {
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
        }
        visitExp(ctx->exp()[i + 1]);
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->exp()[i + 1], "");
        }
    }
    return nullptr;
}

// exp:
// 	NIL
// 	| FALSE
// 	| TRUE
// 	| number
// 	| string
// 	| ELLIPSIS
// 	| prefixexp
// 	| functiondef
// 	| tableconstructor
// 	| exp linkOperator exp
// 	| unaryOperator exp;

antlrcpp::Any FormatVisitor::visitExp(LuaParser::ExpContext* ctx) {
    LOG("visitExp");
    if (ctx->linkOperator() != NULL) {
        visitExp(ctx->exp()[0]);  //
        cur_writer() << commentAfter(ctx->exp()[0], " ");
        cur_writer() << ctx->linkOperator()->getText();
        cur_writer() << commentAfter(ctx->linkOperator(), " ");
        visitExp(ctx->exp()[1]);
    } else if (ctx->unaryOperator() != NULL) {
        cur_writer() << ctx->unaryOperator()->getText();
        if (ctx->unaryOperator()->getText() == "not") {
            cur_writer() << commentAfter(ctx->unaryOperator(), " ");
        } else {
            if (ctx->unaryOperator()->getText() == "-" && ctx->exp().front()->getText()[0] == '-') {
                cur_writer() << commentAfter(ctx->unaryOperator(), " ");
            } else {
                cur_writer() << commentAfter(ctx->unaryOperator(), "");
            }
        }
        visitExp(ctx->exp().front());
    } else if (ctx->prefixexp() != NULL) {
        visitPrefixexp(ctx->prefixexp());
    } else if (ctx->functiondef() != NULL) {
        visitFunctiondef(ctx->functiondef());
    } else if (ctx->tableconstructor() != NULL) {
        visitTableconstructor(ctx->tableconstructor());
    } else {
        cur_writer() << ctx->getText();
    }
    return nullptr;
}

// varOrExp nameAndArgs*;
antlrcpp::Any FormatVisitor::visitPrefixexp(LuaParser::PrefixexpContext* ctx) {
    LOG("visitPrefixexp");
    visitVarOrExp(ctx->varOrExp());
    int n = ctx->nameAndArgs().size();
    bool needWhiteSpace = false;
    if (n > 0) {
        LuaParser::NameAndArgsContext* naaCtx = ctx->nameAndArgs().front();
        if (naaCtx->COLON() == NULL) {
            LuaParser::StringContext* stringContext = naaCtx->args()->string();
            needWhiteSpace = stringContext != NULL;
        }
        cur_writer() << commentAfter(ctx->varOrExp(), needWhiteSpace ? " " : "");
    }
    for (int i = 0; i < n; i++) {
        visitNameAndArgs(ctx->nameAndArgs()[i]);
        if (i != n - 1) {
            needWhiteSpace = false;
            LuaParser::NameAndArgsContext* naaCtx = ctx->nameAndArgs()[i + 1];
            if (naaCtx->COLON() == NULL) {
                LuaParser::StringContext* stringContext = naaCtx->args()->string();
                needWhiteSpace = stringContext != NULL;
            }
            cur_writer() << commentAfter(ctx->nameAndArgs()[i], needWhiteSpace ? " " : "");
        }
    }
    return nullptr;
}

// var | LP exp RP;
antlrcpp::Any FormatVisitor::visitVarOrExp(LuaParser::VarOrExpContext* ctx) {
    LOG("visitVarOrExp");
    if (ctx->exp() != NULL) {
        cur_writer() << ctx->LP()->getText();
        cur_writer() << commentAfter(ctx->LP(), "");
        visitExp(ctx->exp());
        cur_writer() << commentAfter(ctx->exp(), "");
        cur_writer() << ctx->RP()->getText();

    } else {
        visitVar(ctx->var());
    }
    return nullptr;
}

// (NAME | LP exp RP varSuffix) varSuffix*;
antlrcpp::Any FormatVisitor::visitVar(LuaParser::VarContext* ctx) {
    LOG("visitVar");
    int startAt;
    if (ctx->exp() != NULL) {
        cur_writer() << ctx->LP()->getText();
        cur_writer() << commentAfter(ctx->LP(), "");
        visitExp(ctx->exp());
        cur_writer() << commentAfter(ctx->exp(), "");
        cur_writer() << ctx->RP()->getText();
        cur_writer() << commentAfter(ctx->RP(), "");
        visitVarSuffix(ctx->varSuffix().front());
        if (ctx->varSuffix().size() > 1) {
            cur_writer() << commentAfter(ctx->varSuffix().front(), "");
        }
        startAt = 1;
    } else {
        cur_writer() << ctx->NAME()->getText();
        if (ctx->varSuffix().size() > 0) {
            cur_writer() << commentAfter(ctx->NAME(), "");
        }
        startAt = 0;
    }
    int n = ctx->varSuffix().size();
    for (int i = startAt; i < n; i++) {
        visitVarSuffix(ctx->varSuffix()[i]);
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->varSuffix()[i], "");
        }
    }
    return nullptr;
}

// nameAndArgs* (LSB exp RSB | DOT NAME);
antlrcpp::Any FormatVisitor::visitVarSuffix(LuaParser::VarSuffixContext* ctx) {
    LOG("visitVarSuffix");
    for (auto na : ctx->nameAndArgs()) {
        visitNameAndArgs(na);
        cur_writer() << commentAfter(na, "");
    }
    if (ctx->exp() != NULL) {
        string expString = ctx->exp()->getText();
        // if table key is a nested string, keep the whitespace
        // example:
        // x = {}
        // x[ [[key]] ] = "value"
        if (expString.size() > 0 && expString[0] == '[') {
            cur_writer() << ctx->LSB()->getText();
            cur_writer() << commentAfter(ctx->LSB(), " ");
            visitExp(ctx->exp());
            cur_writer() << commentAfter(ctx->exp(), " ");
            cur_writer() << ctx->RSB()->getText();
        } else {
            cur_writer() << ctx->LSB()->getText();
            cur_writer() << commentAfter(ctx->LSB(), "");
            visitExp(ctx->exp());
            cur_writer() << commentAfter(ctx->exp(), "");
            cur_writer() << ctx->RSB()->getText();
        }
    } else {
        cur_writer() << ctx->DOT()->getText();
        cur_writer() << commentAfter(ctx->DOT(), "");
        cur_writer() << ctx->NAME()->getText();
    }
    return nullptr;
}

// (COLON NAME)? args;
antlrcpp::Any FormatVisitor::visitNameAndArgs(LuaParser::NameAndArgsContext* ctx) {
    LOG("visitNameAndArgs");
    if (ctx->COLON() != NULL) {
        cur_writer() << ctx->COLON()->getText();
        cur_writer() << commentAfter(ctx->COLON(), "");
        cur_writer() << ctx->NAME()->getText();
        bool needWhiteSpace = false;
        LuaParser::StringContext* stringContext = ctx->args()->string();
        needWhiteSpace = stringContext != NULL;
        cur_writer() << commentAfter(ctx->NAME(), needWhiteSpace ? " " : "");
        visitArgs(ctx->args());
    } else {
        visitArgs(ctx->args());
    }
    return nullptr;
}

// LP explist? RP | tableconstructor | string;
antlrcpp::Any FormatVisitor::visitArgs(LuaParser::ArgsContext* ctx) {
    LOG("visitArgs");
    if (ctx->LP() != NULL) {
        cur_writer() << ctx->LP()->getText();
        if (ctx->explist() != NULL) {
            if (isParameterSimple(ctx->explist())) {
                cur_writer() << commentAfter(ctx->LP(), "");
                bool temp = chop_down_parameter_;
                chop_down_parameter_ = false;
                visitExplist(ctx->explist());
                chop_down_parameter_ = temp;
                cur_writer() << commentAfter(ctx->explist(), "");
                cur_writer() << ctx->RP()->getText();
            } else {
                cur_writer() << commentAfterNewLine(ctx->LP(), 1);
                cur_writer() << indent();
                bool temp = chop_down_parameter_;
                chop_down_parameter_ = true;
                visitExplist(ctx->explist());
                chop_down_parameter_ = temp;
                cur_writer() << commentAfterNewLine(ctx->explist(), -1);
                cur_writer() << indent();
                cur_writer() << ctx->RP()->getText();
            }
        } else {
            cur_writer() << commentAfter(ctx->LP(), "");
            cur_writer() << ctx->RP()->getText();
        }
    } else {
        visitChildren(ctx);
    }
    return nullptr;
}

// FUNCTION funcbody;
antlrcpp::Any FormatVisitor::visitFunctiondef(LuaParser::FunctiondefContext* ctx) {
    LOG("visitFunctiondef");
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), "");
    visitFuncbody(ctx->funcbody());
    return nullptr;
}

// LP parlist? RP block END
antlrcpp::Any FormatVisitor::visitFuncbody(LuaParser::FuncbodyContext* ctx) {
    LOG("visitFuncbody");
    cur_writer() << ctx->LP()->getText();
    cur_writer() << commentAfter(ctx->LP(), "");
    if (ctx->parlist() != NULL) {
        visitParlist(ctx->parlist());
        cur_writer() << commentAfter(ctx->parlist(), "");
    }
    cur_writer() << ctx->RP()->getText();
    visitBlockAndComment(ctx->RP(), ctx->block());
    cur_writer() << ctx->END()->getText();
    return nullptr;
}

// namelist (COMMA ELLIPSIS)? | ELLIPSIS
antlrcpp::Any FormatVisitor::visitParlist(LuaParser::ParlistContext* ctx) {
    LOG("visitParlist");
    if (ctx->namelist() != NULL) {
        visitNamelist(ctx->namelist());
        if (ctx->COMMA() != NULL) {
            cur_writer() << commentAfter(ctx->namelist(), "");
            cur_writer() << ctx->COMMA()->getText();
            cur_writer() << commentAfter(ctx->COMMA(), " ");
            cur_writer() << ctx->ELLIPSIS()->getText();
        }
    } else {
        cur_writer() << ctx->ELLIPSIS()->getText();
    }

    return nullptr;
}

// LB fieldlist? RB;
antlrcpp::Any FormatVisitor::visitTableconstructor(LuaParser::TableconstructorContext* ctx) {
    LOG("visitTableconstructor");
    cur_writer() << ctx->LB()->getText();
    if (ctx->fieldlist() != NULL) {
        if (isTableSimple(ctx)) {
            cur_writer() << commentAfter(ctx->LB(), "");
            bool temp = chop_down_table_;
            chop_down_table_ = false;
            visitFieldlist(ctx->fieldlist());
            chop_down_table_ = temp;
            cur_writer() << commentAfter(ctx->fieldlist(), "");
            cur_writer() << ctx->RB()->getText();
        } else {
            cur_writer() << commentAfterNewLine(ctx->LB(), 1);
            bool temp = chop_down_table_;
            chop_down_table_ = true;
            visitFieldlist(ctx->fieldlist());
            chop_down_table_ = temp;
            if (config_.extra_sep_at_table_end() && ctx->fieldlist()->field().size() > 0) {
                cur_writer() << config_.table_sep();
            }
            cur_writer() << commentAfterNewLine(ctx->fieldlist(), -1);
            cur_writer() << indent();
            cur_writer() << ctx->RB()->getText();
        }
    } else {
        string comment = commentAfter(ctx->LB(), "");
        // if comment contains line break, then keep it.
        if (comment.find("\n") != string::npos) {
            cur_writer() << commentAfterNewLine(ctx->LB(), 1);
            cur_writer() << decIndent();
            cur_writer() << ctx->RB()->getText();
        } else {
            // comment not contains line break, format the table to one line.
            cur_writer() << commentAfter(ctx->LB(), "");
            cur_writer() << ctx->RB()->getText();
        }
    }
    return nullptr;
}

// field (fieldsep field)* fieldsep?;
antlrcpp::Any FormatVisitor::visitFieldlist(LuaParser::FieldlistContext* ctx) {
    LOG("visitFieldlist");
    auto fields = ctx->field();
    auto seps = ctx->fieldsep();
    visitField(fields.front());
    if (fields.size() > 1 || seps.size() > 0) {
        cur_writer() << commentAfter(fields.front(), "");
        int n = fields.size();
        for (int i = 1; i < n; i++) {
            visitFieldsep(seps[i - 1]);
            if (chop_down_table_) {
                cur_writer() << commentAfterNewLine(seps[i - 1], 0);
            } else {
                cur_writer() << commentAfter(seps[i - 1], " ");
            }
            visitField(fields[i]);
            if (i != n - 1 || seps.size() == n) {
                cur_writer() << commentAfter(fields[i], "");
            }
        }
        if (seps.size() == n && !config_.extra_sep_at_table_end()) {
            visitFieldsep(seps.back());
        }
    }

    return nullptr;
}

// LSB exp RSB EQL exp | NAME EQL exp | exp;
antlrcpp::Any FormatVisitor::visitField(LuaParser::FieldContext* ctx) {
    if (ctx->LSB() != NULL) {
        if (chop_down_table_) {
            cur_writer() << indent();
        }
        cur_writer() << ctx->LSB()->getText();
        cur_writer() << commentAfter(ctx->LSB(), "");
        visitExp(ctx->exp()[0]);
        cur_writer() << commentAfter(ctx->exp()[0], "");
        cur_writer() << ctx->RSB()->getText();
        cur_writer() << commentAfter(ctx->RSB(), " ");
        cur_writer() << ctx->EQL()->getText();
        cur_writer() << commentAfter(ctx->EQL(), " ");
        visitExp(ctx->exp()[1]);
    } else if (ctx->NAME() != NULL) {
        if (chop_down_table_) {
            cur_writer() << indent();
        }
        cur_writer() << ctx->NAME()->getText();
        cur_writer() << commentAfter(ctx->NAME(), " ");
        cur_writer() << ctx->EQL()->getText();
        cur_writer() << commentAfter(ctx->EQL(), " ");
        visitExp(ctx->exp().front());
    } else {
        if (chop_down_table_) {
            cur_writer() << indent();
        }
        visitExp(ctx->exp().front());
    }
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitFieldsep(LuaParser::FieldsepContext* context) {
    cur_writer() << config_.table_sep();
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitChildren(tree::ParseTree* node) {
    size_t n = node->children.size();
    for (size_t i = 0; i < n; i++) {
        node->children[i]->accept(this);
    }
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitTerminal(tree::TerminalNode* node) {
    cur_writer() << node->getText();
    return nullptr;
}

// If both of the following conditions are met, the custom semicolon should not be deleted:
// 1. There is a variable token or function call token at end of previous statement
//    e.g. 'local x = y' or 'y:func()'
// 2. next statement is start with a '(', like '(function() end)()'
bool FormatVisitor::shouldKeepSemicolon(ParserRuleContext* ctx, tree::TerminalNode* node) {
    if (ctx == NULL) {
        return false;
    }

    bool returnVar = false;

    // varDecl or localVarDecl
    // 'local x, y = 1, print;'
    LuaParser::ExplistContext* explistCtx = dynamic_cast<LuaParser::ExplistContext*>(ctx);
    if (explistCtx != NULL) {
        LuaParser::ExpContext* lastExp = explistCtx->exp().back();
        while (!lastExp->exp().empty()) {
            lastExp = lastExp->exp().back();
        }
        returnVar = lastExp->prefixexp() != NULL;
    }

    // repeat statement
    // repeat
    //     print("123")
    // until y;
    LuaParser::ExpContext* expCtx = dynamic_cast<LuaParser::ExpContext*>(ctx);
    if (expCtx != NULL) {
        LuaParser::ExpContext* lastExp = expCtx;
        while (!lastExp->exp().empty()) {
            lastExp = lastExp->exp().back();
        }
        returnVar = lastExp->prefixexp() != NULL;
    }

    // function call
    LuaParser::NameAndArgsContext* naCtx = dynamic_cast<LuaParser::NameAndArgsContext*>(ctx);
    if (naCtx != NULL) {
        returnVar = true;
    }

    if (!returnVar) {
        return false;
    }

    int idx = node->getSymbol()->getTokenIndex();
    bool startWithLP = false;
    for (int i = idx + 1; i < tokens_.size(); i++) {
        size_t type = tokens_[i]->getType();
        // ignore comments and white space
        if (type == LuaLexer::COMMENT ||       //
            type == LuaLexer::LINE_COMMENT ||  //
            type == LuaLexer::WS) {
            continue;
        }

        startWithLP = type == LuaLexer::LP;
        break;
    }
    return startWithLP;
}

// Judge a rule context has force linebreak
bool FormatVisitor::hasHardLineBreak(ParserRuleContext* ctx) {
    int l = ctx->getStart()->getTokenIndex();
    int r = ctx->getStop()->getTokenIndex();

    for (int i = l + 1; i < tokens_.size() && i < r; i++) {
        auto token = tokens_[i];
        if (token->getType() == LuaLexer::COMMENT) {
            string str = token->getText();
            if (str.find('\n') != string::npos) {
                return true;
            }
        } else if (token->getType() == LuaLexer::LINE_COMMENT) {
            return true;
        }
    }
    return false;
}

bool FormatVisitor::needKeepBlockOneLine(tree::ParseTree* previousNode, LuaParser::BlockContext* ctx) {
    if (!config_.keep_simple_block_one_line()) {
        return false;
    }
    int stats = 0;
    for (auto& s : ctx->stat()) {
        if (s->SEMI() == NULL) {
            stats++;
        }
    }
    if (ctx->retstat() != NULL) {
        stats++;
    }
    // block contains more than one statement.
    if (stats > 1) {
        return false;
    }

    // block too long
    if (ctx->getText().size() >= config_.chop_down_block()) {
        return false;
    }

    // block contains a line break.
    string preComment = commentAfter(previousNode, "");
    if (preComment.find("\n") != string::npos) {
        return false;
    }
    pushWriter();
    visitBlock(ctx);
    string blockContent = popWriter();
    if (blockContent.find("\n") != string::npos) {
        return false;
    }
    string postComment = commentAfter(ctx, "");
    if (postComment.find("\n") != string::npos) {
        return false;
    }
    return true;
}

bool FormatVisitor::isBlockEmpty(LuaParser::BlockContext* ctx) { return ctx->stat().size() == 0 && ctx->retstat() == NULL; }

void FormatVisitor::visitBlockAndComment(tree::ParseTree* previousNode, LuaParser::BlockContext* ctx) {
    LOG("visitBlockAndComment");
    bool oneline = needKeepBlockOneLine(previousNode, ctx);
    if (oneline) {
        cur_writer() << commentAfter(previousNode, " ");
        bool temp = chop_down_block_;
        chop_down_block_ = false;
        visitBlock(ctx);
        chop_down_block_ = temp;
        if (!isBlockEmpty(ctx)) {
            cur_writer() << commentAfter(ctx, " ");
        }
    } else {
        cur_writer() << commentAfterNewLine(previousNode, 1);
        bool temp = chop_down_block_;
        chop_down_block_ = true;
        visitBlock(ctx);
        chop_down_block_ = temp;
        cur_writer() << commentAfterNewLine(ctx, -1);
        cur_writer() << indent();
    }
}

// Judge a table is 'simple table' or not
bool FormatVisitor::isTableSimple(LuaParser::TableconstructorContext* ctx) {
    // table too long
    if (ctx->getText().size() >= config_.chop_down_table()) {
        return false;
    }

    // table contains line break.
    string preComment = commentAfter(ctx->LB(), "");
    if (preComment.find("\n") != string::npos) {
        return false;
    }
    if (hasHardLineBreak(ctx)) {
        return false;
    }
    bool temp = chop_down_table_;
    chop_down_table_ = false;
    pushWriter();
    visitFieldlist(ctx->fieldlist());
    string fieldContext = popWriter();
    chop_down_table_ = temp;
    if (fieldContext.find("\n") != string::npos) {
        return false;
    }
    string postComment = commentAfter(ctx->fieldlist(), "");
    if (postComment.find("\n") != string::npos) {
        return false;
    }
    return true;
}

// Judge explist need chop down or not
bool FormatVisitor::isParameterSimple(LuaParser::ExplistContext* ctx) {
    if (ctx->exp().size() <= 1) {
        return true;
    }
    if (ctx->getText().size() >= config_.chop_down_parameter()) {
        return false;
    }
    if (hasHardLineBreak(ctx)) {
        return false;
    }

    return true;
}

string FormatVisitor::indent() {
    stringstream ss;
    for (int i = 0; i < indent_; i++) {
        ss << config_.indent();
    }
    return ss.str();
}

string FormatVisitor::incIndent() {
    indent_++;
    return indent();
}

string FormatVisitor::decIndent() {
    indent_--;
    return indent();
}
