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
string FormatVisitor::commentAfterNewLine(tree::ParseTree* node, NewLineIndent newLineIndent) {
    ParserRuleContext* ctx = dynamic_cast<ParserRuleContext*>(node);
    if (ctx != NULL && ctx->children.size() == 0) {
        if (newLineIndent == INC_INDENT) {
            incIndent();
        } else if (newLineIndent == INC_CONTINUATION_INDENT) {
            incContinuationIndent();
        }
        return "";
    }
    int l = lastNodeIndex(node);
    if (newLineIndent == INC_INDENT) {
        incIndent();
    } else if (newLineIndent == INC_CONTINUATION_INDENT) {
        incContinuationIndent();
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
    if (newLineIndent == DEC_INDENT) {
        decIndent();
    } else if (newLineIndent == DEC_CONTINUATION_INDENT) {
        decContinuationIndent();
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

int FormatVisitor::cur_columns() { return (*writers_.back()).columns().back(); }

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
    string comment = commentAfterNewLine(ctx->block(), NONE_INDENT);

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
                string comment = commentAfterNewLine(stats[i], NONE_INDENT);
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
    visitExplist(ctx->explist());
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
    chainedMethodCallHasIncIndent_.push_back(false);
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
    if (chainedMethodCallHasIncIndent_.back()) {
        decContinuationIndent();
    }
    chainedMethodCallHasIncIndent_.pop_back();
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
    cur_writer() << commentAfterNewLine(ctx->THEN().front(), INC_INDENT);
    visitBlock(ctx->block().front());
    cur_writer() << commentAfterNewLine(ctx->block().front(), DEC_INDENT);
    int n = ctx->ELSEIF().size();
    for (int i = 0; i < n; i++) {
        cur_writer() << indent();
        cur_writer() << ctx->ELSEIF()[i]->getText();
        cur_writer() << commentAfter(ctx->ELSEIF()[i], " ");
        visitExp(ctx->exp()[i + 1]);
        cur_writer() << commentAfter(ctx->exp()[i + 1], " ");
        cur_writer() << ctx->THEN()[i + 1]->getText();
        cur_writer() << commentAfterNewLine(ctx->THEN()[i + 1], INC_INDENT);
        visitBlock(ctx->block()[i + 1]);
        cur_writer() << commentAfterNewLine(ctx->block()[i + 1], DEC_INDENT);
    }
    if (ctx->ELSE() != NULL) {
        cur_writer() << indent();
        cur_writer() << ctx->ELSE()->getText();
        cur_writer() << commentAfterNewLine(ctx->ELSE(), INC_INDENT);
        visitBlock(ctx->block().back());
        cur_writer() << commentAfterNewLine(ctx->block().back(), DEC_INDENT);
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
    visitExplist(ctx->explist());
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
        visitExplist(ctx->explist());
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
        visitExplist(ctx->explist());
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
    int n = ctx->COMMA().size();

    int firstParameterLength = ctx->NAME().front()->getText().size();
    bool hasIncIndent = false;
    if (n > 0) firstParameterLength++;  // calc a ',' if exp > 1
    if (cur_columns() + firstParameterLength > config_.column_limit()) {
        cur_writer() << "\n";
        if (!hasIncIndent) {
            incIndent();
            cur_writer() << indent();
            hasIncIndent = true;
        }
        if (config_.align_parameter()) {
            firstParameterColumn_.push_back(cur_columns());
        }
        cur_writer() << ctx->NAME().front()->getText();
    } else {
        if (config_.align_parameter()) {
            firstParameterColumn_.push_back(cur_columns());
        }
        cur_writer() << ctx->NAME().front()->getText();
    }
    if (n > 0) {
        cur_writer() << commentAfter(ctx->NAME().front(), "");
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        cur_writer() << commentAfter(ctx->COMMA()[i], " ");

        string str = ctx->NAME()[i + 1]->getText();
        int length = str.size();
        if (i != n - 1) length++;  // calc a ',' if exp > 1
        if (cur_columns() + length > config_.column_limit()) {
            // erase last whitespace when there is a line break
            if (isLastWhiteSpace(cur_writer().ss())) {
                cur_writer().ss().seekp(-1, ios::end);
            }
            cur_writer() << "\n";
            if (config_.align_parameter()) {
                for (int i = 0; i < firstParameterColumn_.back(); i++) {
                    cur_writer() << " ";
                }
            } else {
                if (hasIncIndent) {
                    cur_writer() << indent();
                } else {
                    incIndent();
                    cur_writer() << indent();
                    hasIncIndent = true;
                }
            }
            cur_writer() << str;
        } else {
            cur_writer() << str;
        }
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->NAME()[i + 1], "");
        }
    }
    if (hasIncIndent) {
        decContinuationIndent();
    }
    if (config_.align_parameter()) {
        firstParameterColumn_.pop_back();
    }
    return nullptr;
}

// exp (COMMA exp)*;
antlrcpp::Any FormatVisitor::visitExplist(LuaParser::ExplistContext* ctx) {
    LOG("visitExplist");
    int n = ctx->COMMA().size();
    pushWriter();
    visitExp(ctx->exp().front());
    int firstExpLength = cur_writer().columns().front();
    popWriter();
    bool hasIncIndent = false;
    if (n > 0) firstExpLength++;  // calc a ',' if exp > 1
    if (cur_columns() + firstExpLength > config_.column_limit()) {
        if (isLastWhiteSpace(cur_writer().ss())) {
            cur_writer().ss().seekp(-1, ios::end);
        }
        cur_writer() << "\n";
        if (!hasIncIndent) {
            incIndent();
            cur_writer() << indent();
            hasIncIndent = true;
        }
        if (config_.align_args()) {
            firstArgsColumn_.push_back(cur_columns());
        }
        visitExp(ctx->exp().front());
    } else {
        if (config_.align_args()) {
            firstArgsColumn_.push_back(cur_columns());
        }
        visitExp(ctx->exp().front());
    }
    if (n > 0) {
        cur_writer() << commentAfter(ctx->exp().front(), "");
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        cur_writer() << commentAfter(ctx->COMMA()[i], " ");
        pushWriter();
        visitExp(ctx->exp()[i + 1]);
        int expLength = cur_writer().columns().front();
        popWriter();
        if (i != n - 1) expLength++;  // calc a ',' if exp > 1
        if (cur_columns() + expLength > config_.column_limit()) {
            // erase last whitespace when there is a line break
            if (isLastWhiteSpace(cur_writer().ss())) {
                cur_writer().ss().seekp(-1, ios::end);
            }
            cur_writer() << "\n";
            if (config_.align_args()) {
                for (int i = 0; i < firstArgsColumn_.back(); i++) {
                    cur_writer() << " ";
                }
            } else {
                if (hasIncIndent) {
                    cur_writer() << indent();
                } else {
                    incIndent();
                    cur_writer() << indent();
                    hasIncIndent = true;
                }
            }
            visitExp(ctx->exp()[i + 1]);
        } else {
            visitExp(ctx->exp()[i + 1]);
        }
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->exp()[i + 1], "");
        }
    }
    if (hasIncIndent) {
        decContinuationIndent();
    }
    if (config_.align_args()) {
        firstArgsColumn_.pop_back();
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
        visitExp(ctx->exp()[0]);
        bool hasIncIndent = false;
        if (config_.break_after_operator()) {
            cur_writer() << commentAfter(ctx->exp()[0], " ");
            cur_writer() << ctx->linkOperator()->getText();
            pushWriter();
            visitExp(ctx->exp()[1]);
            int length = cur_writer().columns().front();
            popWriter();
            if (cur_columns() + length > config_.column_limit()) {
                cur_writer() << commentAfterNewLine(ctx->linkOperator(), INC_CONTINUATION_INDENT);
                hasIncIndent = true;
            } else {
                cur_writer() << commentAfter(ctx->linkOperator(), " ");
            }
            visitExp(ctx->exp()[1]);
        } else {
            pushWriter();
            cur_writer() << ctx->linkOperator()->getText();
            cur_writer() << commentAfter(ctx->linkOperator(), " ");
            visitExp(ctx->exp()[1]);
            int length = cur_writer().columns().front();
            popWriter();
            if (cur_columns() + length > config_.column_limit()) {
                cur_writer() << commentAfterNewLine(ctx->exp()[0], INC_CONTINUATION_INDENT);
                hasIncIndent = true;
            } else {
                cur_writer() << commentAfter(ctx->exp()[0], " ");
            }
            cur_writer() << ctx->linkOperator()->getText();
            cur_writer() << commentAfter(ctx->linkOperator(), " ");
            visitExp(ctx->exp()[1]);
        }
        if (hasIncIndent) {
            decContinuationIndent();
        }
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
    chainedMethodCallHasIncIndent_.push_back(false);
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
    if (chainedMethodCallHasIncIndent_.back()) {
        decContinuationIndent();
    }
    chainedMethodCallHasIncIndent_.pop_back();
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
        if (ctx->varSuffix().size() > 1) {
            nextVarSuffixContext_.push_back(ctx->varSuffix()[1]);
        } else {
            nextVarSuffixContext_.push_back(nullptr);
        }
        visitVarSuffix(ctx->varSuffix().front());
        if (ctx->varSuffix().size() > 1) {
            cur_writer() << commentAfter(ctx->varSuffix().front(), "");
        }
        nextVarSuffixContext_.pop_back();
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
        if (i != n - 1) {
            nextVarSuffixContext_.push_back(ctx->varSuffix()[i + 1]);
        } else {
            nextVarSuffixContext_.push_back(nullptr);
        }
        visitVarSuffix(ctx->varSuffix()[i]);
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->varSuffix()[i], "");
        }
        nextVarSuffixContext_.pop_back();
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
        pushWriter();
        cur_writer() << ctx->DOT()->getText();
        cur_writer() << commentAfter(ctx->DOT(), "");
        cur_writer() << ctx->NAME()->getText();

        // find args of NAME
        LuaParser::VarContext* varCtx = dynamic_cast<LuaParser::VarContext*>(ctx->parent);
        const vector<LuaParser::VarSuffixContext*>& arr = varCtx->varSuffix();
        int index = find(arr.begin(), arr.end(), ctx) - arr.begin();
        if (index + 1 < arr.size()) {
            for (auto na : arr[index + 1]->nameAndArgs()) {
                visitNameAndArgs(na);
                cur_writer() << commentAfter(na, "");
            }
        } else {
            LuaParser::VarOrExpContext* varOrExpCtx = dynamic_cast<LuaParser::VarOrExpContext*>(varCtx->parent);
            // parent maybe VarlistContext
            if (varOrExpCtx != nullptr) {
                tree::ParseTree* parent = varOrExpCtx->parent;

                LuaParser::PrefixexpContext* peCtx = dynamic_cast<LuaParser::PrefixexpContext*>(parent);
                if (peCtx != nullptr) {
                    for (auto na : peCtx->nameAndArgs()) {
                        visitNameAndArgs(na);
                        cur_writer() << commentAfter(na, "");
                    }
                }

                LuaParser::FunctioncallContext* fcCtx = dynamic_cast<LuaParser::FunctioncallContext*>(parent);
                if (fcCtx != nullptr) {
                    for (auto na : fcCtx->nameAndArgs()) {
                        visitNameAndArgs(na);
                        cur_writer() << commentAfter(na, "");
                    }
                }
            }
        }

        int length = cur_writer().columns().front();
        popWriter();
        if (cur_columns() + length > config_.column_limit()) {
            cur_writer() << "\n";
            if (chainedMethodCallHasIncIndent_.empty()) {
                // chainedMethodCallHasIncIndent_ is empty when visit var in varlist
                cur_writer() << indent();
            } else {
                if (chainedMethodCallHasIncIndent_.back()) {
                    cur_writer() << indent();
                } else {
                    incIndent();
                    cur_writer() << indent();
                    chainedMethodCallHasIncIndent_[chainedMethodCallHasIncIndent_.size() - 1] = true;
                }
            }
        }
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
        pushWriter();
        cur_writer() << ctx->COLON()->getText();
        cur_writer() << commentAfter(ctx->COLON(), "");
        cur_writer() << ctx->NAME()->getText();
        bool needWhiteSpace = false;
        LuaParser::StringContext* stringContext = ctx->args()->string();
        needWhiteSpace = stringContext != NULL;
        cur_writer() << commentAfter(ctx->NAME(), needWhiteSpace ? " " : "");
        visitArgs(ctx->args());
        int length = cur_writer().columns().front();
        popWriter();
        if (cur_columns() + length > config_.column_limit()) {
            cur_writer() << "\n";
            if (chainedMethodCallHasIncIndent_.back()) {
                cur_writer() << indent();
            } else {
                incIndent();
                cur_writer() << indent();
                chainedMethodCallHasIncIndent_[chainedMethodCallHasIncIndent_.size() - 1] = true;
            }
        }
        cur_writer() << ctx->COLON()->getText();
        cur_writer() << commentAfter(ctx->COLON(), "");
        cur_writer() << ctx->NAME()->getText();
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
            cur_writer() << commentAfter(ctx->LP(), "");
            visitExplist(ctx->explist());
            cur_writer() << commentAfter(ctx->explist(), "");
            cur_writer() << ctx->RP()->getText();
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
    if (ctx->parlist() != NULL) {
        bool breakAfterLp = false;
        bool newline = false;
        pushWriter();
        visitParlist(ctx->parlist());
        int length = cur_writer().columns().front();
        popWriter();
        if (cur_columns() + length > config_.column_limit()) {
            newline = true;
            breakAfterLp = config_.break_after_functiondef_lp();
        }
        if (breakAfterLp) {
            cur_writer() << commentAfterNewLine(ctx->LP(), INC_INDENT);
            cur_writer() << indent();
        } else {
            cur_writer() << commentAfter(ctx->LP(), "");
        }

        visitParlist(ctx->parlist());

        if (config_.break_before_functiondef_rp()) {
            if (breakAfterLp) {
                cur_writer() << commentAfterNewLine(ctx->parlist(), DEC_INDENT);
                cur_writer() << indent();
            } else {
                cur_writer() << commentAfter(ctx->parlist(), "");
            }
        } else {
            if (breakAfterLp) decIndent();
            cur_writer() << commentAfter(ctx->parlist(), "");
        }
    } else {
        cur_writer() << commentAfter(ctx->LP(), "");
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
        bool containsKv = false;
        bool chopDown = false;
        if (config_.chop_down_kv_table()) {
            for (auto f : ctx->fieldlist()->field()) {
                if (f->EQL() != nullptr) {
                    containsKv = true;
                    break;
                }
            }
        }
        bool temp = chop_down_table_;
        pushWriter();
        cur_writer() << commentAfter(ctx->LB(), "");
        chop_down_table_ = false;
        visitFieldlist(ctx->fieldlist());
        chop_down_table_ = temp;
        cur_writer() << commentAfter(ctx->fieldlist(), "");
        int length = cur_writer().columns().front();
        int lines = cur_writer().lines();
        popWriter();
        bool breakAfterLb = false;
        if (cur_columns() + length > config_.column_limit() || lines > 1) {
            breakAfterLb = config_.break_after_table_lb();
            chopDown = config_.chop_down_kv_table() && containsKv;
        }
        if (chopDown) {
            cur_writer() << commentAfterNewLine(ctx->LB(), INC_INDENT);
            cur_writer() << indent();
            chop_down_table_ = true;
        } else {
            if (breakAfterLb) {
                cur_writer() << commentAfterNewLine(ctx->LB(), INC_INDENT);
                cur_writer() << indent();
            } else {
                cur_writer() << commentAfter(ctx->LB(), "");
            }
            chop_down_table_ = false;
        }
        visitFieldlist(ctx->fieldlist());
        if (config_.extra_sep_at_table_end() && ctx->fieldlist()->field().size() > 0) {
            cur_writer() << config_.table_sep();
        }
        if (chopDown) {
            cur_writer() << commentAfterNewLine(ctx->fieldlist(), DEC_INDENT);
            cur_writer() << indent();
        } else {
            if (breakAfterLb) {
                if (config_.break_before_table_rb()) {
                    cur_writer() << commentAfterNewLine(ctx->fieldlist(), DEC_INDENT);
                    cur_writer() << indent();
                } else {
                    decContinuationIndent();
                    cur_writer() << commentAfter(ctx->fieldlist(), "");
                }
            } else {
                cur_writer() << commentAfter(ctx->fieldlist(), "");
            }
        }
        cur_writer() << ctx->RB()->getText();
        chop_down_table_ = temp;
    } else {
        string comment = commentAfter(ctx->LB(), "");
        // if comment contains line break, then keep it.
        if (comment.find("\n") != string::npos) {
            cur_writer() << commentAfterNewLine(ctx->LB(), INC_INDENT);
            decIndent();
            cur_writer() << indent();
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
    int n = ctx->field().size();
    int sn = ctx->fieldsep().size();
    if (config_.align_table_field()) {
        firstTableFieldColumn_.push_back(cur_columns());
    }
    bool hasIncIndent = isLastWhiteSpace(cur_writer().ss());
    bool customIncIndent = false;
    visitField(ctx->field().front());
    if (n > 1 || sn > 0) {
        cur_writer() << commentAfter(ctx->field().front(), "");
    }
    for (int i = 1; i < n; i++) {
        if (chop_down_table_) {
            visitFieldsep(ctx->fieldsep()[i - 1]);
            cur_writer() << commentAfterNewLine(ctx->fieldsep()[i - 1], NONE_INDENT);
            cur_writer() << indent();
            visitField(ctx->field()[i]);
            if (i != n - 1 || ctx->fieldsep().size() == n) {
                cur_writer() << commentAfter(ctx->field()[i], "");
            }
            continue;
        }
        visitFieldsep(ctx->fieldsep()[i - 1]);
        pushWriter();
        cur_writer() << commentAfter(ctx->fieldsep()[i - 1], " ");
        visitField(ctx->field()[i]);
        int length = cur_writer().columns().front();
        popWriter();
        if (i != n - 1 || config_.extra_sep_at_table_end()) length++;  // calc a ',' if exp > 1
        if (cur_columns() + length > config_.column_limit()) {
            if (config_.align_table_field()) {
                cur_writer() << commentAfter(ctx->fieldsep()[i - 1], " ");
                // erase last whitespace when there is a line break
                if (isLastWhiteSpace(cur_writer().ss())) {
                    cur_writer().ss().seekp(-1, ios::end);
                }
                cur_writer() << "\n";
                for (int i = 0; i < firstTableFieldColumn_.back(); i++) {
                    cur_writer() << " ";
                }
            } else {
                if (hasIncIndent) {
                    cur_writer() << commentAfterNewLine(ctx->fieldsep()[i - 1], NONE_INDENT);
                    cur_writer() << indent();
                } else {
                    cur_writer() << commentAfterNewLine(ctx->fieldsep()[i - 1], INC_INDENT);
                    cur_writer() << indent();
                    customIncIndent = true;
                }
            }
            visitField(ctx->field()[i]);
        } else {
            cur_writer() << commentAfter(ctx->fieldsep()[i - 1], " ");
            visitField(ctx->field()[i]);
        }
        if (i != n - 1 || ctx->fieldsep().size() == n) {
            cur_writer() << commentAfter(ctx->field()[i], "");
        }
    }
    if (ctx->fieldsep().size() == n && !config_.extra_sep_at_table_end()) {
        visitFieldsep(ctx->fieldsep().back());
    }
    if (config_.align_table_field()) {
        firstTableFieldColumn_.pop_back();
    }
    if (customIncIndent) {
        decIndent();
    }
    return nullptr;
}

// LSB exp RSB EQL exp | NAME EQL exp | exp;
antlrcpp::Any FormatVisitor::visitField(LuaParser::FieldContext* ctx) {
    if (ctx->LSB() != NULL) {
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
        cur_writer() << ctx->NAME()->getText();
        cur_writer() << commentAfter(ctx->NAME(), " ");
        cur_writer() << ctx->EQL()->getText();
        cur_writer() << commentAfter(ctx->EQL(), " ");
        visitExp(ctx->exp().front());
    } else {
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

    // block contains a line break.
    string preComment = commentAfter(previousNode, "");
    if (preComment.find("\n") != string::npos) {
        return false;
    }
    pushWriter();
    visitBlock(ctx);
    int length = cur_writer().columns().front();
    int lines = cur_writer().lines();
    popWriter();
    if (cur_columns() + length > config_.column_limit() || lines > 1) {
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
        cur_writer() << commentAfterNewLine(previousNode, INC_INDENT);
        bool temp = chop_down_block_;
        chop_down_block_ = true;
        visitBlock(ctx);
        chop_down_block_ = temp;
        cur_writer() << commentAfterNewLine(ctx, DEC_INDENT);
        cur_writer() << indent();
    }
}

string FormatVisitor::indent() {
    stringstream ss;
    for (int i = 0; i < indent_; i++) {
        ss << " ";
    }
    return ss.str();
}

void FormatVisitor::incIndent() {
    LOG("incIndent");
    indent_ += config_.indent_width();
}

void FormatVisitor::decIndent() {
    LOG("decIndent");
    indent_ -= config_.indent_width();
}

void FormatVisitor::incContinuationIndent() {
    LOG("incContinuationIndent");
    indent_ += config_.continuation_indent_width();
}

void FormatVisitor::decContinuationIndent() {
    LOG("decContinuationIndent");
    indent_ -= config_.continuation_indent_width();
}
