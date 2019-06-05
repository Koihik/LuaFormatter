#include <iostream>

#include "FormatVisitor.h"
#include "LuaLexer.h"

using namespace std;
using namespace antlr4;

// #define LOG_FLAG

#ifdef LOG_FLAG
#define LOGVAR(arg)                                      \
    for (int i = 0; i < logIndentSize; i++) cout << " "; \
    cout << "" #arg " = " << (arg) << endl
#define LOG(arg)                                         \
    for (int i = 0; i < logIndentSize; i++) cout << " "; \
    cout << arg << endl
static int logIndentSize = 0;
#define LOG_FUNCTION_BEGIN(arg)                          \
    logIndentSize++;                                     \
    for (int i = 0; i < logIndentSize; i++) cout << " "; \
    cout << arg << " begin.." << endl
#define LOG_FUNCTION_END(arg)                            \
    for (int i = 0; i < logIndentSize; i++) cout << " "; \
    logIndentSize--;                                     \
    cout << arg << " end.." << endl
#else
#define LOG(arg)
#define LOGVAR(arg)
#define LOG_FUNCTION_BEGIN(arg)
#define LOG_FUNCTION_END(arg)
#endif

static bool isBlankChar(char c) { return c == ' ' || c == '\t' || c == '\u000C'; }

int firstNodeIndex(tree::ParseTree* t) {
    if (t == NULL) {
        return -1;
    }
    ParserRuleContext* c = dynamic_cast<ParserRuleContext*>(t);
    if (c != NULL) {
        return c->getStart()->getTokenIndex();
    }
    tree::TerminalNode* n = dynamic_cast<tree::TerminalNode*>(t);
    if (n != NULL) {
        return n->getSymbol()->getTokenIndex();
    }
    return INT_MAX;
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
// NewLineIndent: inc or dec indent after a line break.
string FormatVisitor::commentAfterNewLine(tree::ParseTree* node, NewLineIndent newLineIndent) {
    ParserRuleContext* ctx = dynamic_cast<ParserRuleContext*>(node);
    if (ctx != NULL && ctx->children.size() == 0) {
        if (newLineIndent == INC_INDENT) {
            incIndent();
        } else if (newLineIndent == INC_CONTINUATION_INDENT) {
            incContinuationIndent();
        } else if (newLineIndent == DEC_INDENT) {
            decIndent();
        } else if (newLineIndent == DEC_CONTINUATION_INDENT) {
            decContinuationIndent();
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

void FormatVisitor::pushWriter() {
    SourceWriter* writer = new SourceWriter(0);
    if (!writers_.empty()) {
        writer->set_volatile(true);
    }
    writers_.push_back(writer);
}

void FormatVisitor::pushWriterWithColumn() {
    SourceWriter* writer = new SourceWriter(cur_columns());
    if (!writers_.empty()) {
        writer->set_volatile(true);
    }
    writers_.push_back(writer);
}

void FormatVisitor::popWriter() {
    SourceWriter* writer = writers_.back();
    writers_.pop_back();
    delete writer;
}

bool FormatVisitor::fastTestColumnLimit(tree::ParseTree* node) {
    int columns = cur_columns();
    for (int i = firstNodeIndex(node), n = lastNodeIndex(node); i <= n; i++) {
        if (tokens_[i]->getType() == LuaLexer::COMMENT || tokens_[i]->getType() == LuaLexer::LINE_COMMENT ||
            tokens_[i]->getType() == LuaLexer::WS) {
            continue;
        }
        columns += tokens_[i]->getText().size();
        if (columns > config_.column_limit()) {
            return true;
        }
    }
    return false;
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
    LOG_FUNCTION_BEGIN("visitChunk");
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
    string ret = cur_writer().str();
    popWriter();
    LOG_FUNCTION_END("visitChunk");
    return ret;
}

// stat* retstat?
antlrcpp::Any FormatVisitor::visitBlock(LuaParser::BlockContext* ctx) {
    LOG_FUNCTION_BEGIN("visitBlock");
    auto stats = ctx->stat();
    int n = stats.size();
    for (int i = 0; i < n; i++) {
        bool isSemi = stats[i]->SEMI() != NULL;
        bool needComment = i < n - 1 || ctx->retstat() != NULL;
        bool isNextSemi = i + 1 < n && stats[i + 1]->SEMI() != NULL;
        if (!isSemi) {
            visitStat(stats[i]);
        }
        bool previousWhiteSpace = isSemi || cur_writer().isLastCharWhiteSpace();
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
    if (ctx->retstat() == NULL && cur_writer().isLastCharWhiteSpace()) {
        string blockComment = commentAfter(ctx, "");
        if (blockComment.size() > 0 && blockComment[0] == ' ') {
            cur_writer().ss().seekp(-1, ios::end);
        }
    }
    LOG_FUNCTION_END("visitBlock");
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitStat(LuaParser::StatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitStat");
    if (chop_down_block_) {
        cur_writer() << indent();
    }
    visitChildren(ctx);
    LOG_FUNCTION_END("visitStat");
    return nullptr;
}

// varlist EQL explist;
antlrcpp::Any FormatVisitor::visitVarDecl(LuaParser::VarDeclContext* ctx) {
    LOG_FUNCTION_BEGIN("visitVarDecl");
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
    LOG_FUNCTION_END("visitVarDecl");
    return nullptr;
}

// varOrExp nameAndArgs+;
antlrcpp::Any FormatVisitor::visitFunctioncall(LuaParser::FunctioncallContext* ctx) {
    LOG_FUNCTION_BEGIN("visitFunctioncall");
    chainedMethodCallHasIncIndent_.push_back(false);
    chainedMethodCallIsFirst_.push_back(false);
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
    chainedMethodCallIsFirst_.pop_back();
    LOG_FUNCTION_END("visitFunctioncall");
    return nullptr;
}

// GOTO NAME;
antlrcpp::Any FormatVisitor::visitGotoStat(LuaParser::GotoStatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitGotoStat");
    cur_writer() << ctx->GOTO()->getText();
    cur_writer() << commentAfter(ctx->GOTO(), " ");
    cur_writer() << ctx->NAME()->getText();
    LOG_FUNCTION_END("visitGotoStat");
    return nullptr;
}

// DO block END;
antlrcpp::Any FormatVisitor::visitDoStat(LuaParser::DoStatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitDoStat");
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block());
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END("visitDoStat");
    return nullptr;
}

// WHILE exp DO block END;
antlrcpp::Any FormatVisitor::visitWhileStat(LuaParser::WhileStatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitWhileStat");
    cur_writer() << ctx->WHILE()->getText();
    cur_writer() << commentAfter(ctx->WHILE(), " ");
    visitExp(ctx->exp());
    cur_writer() << commentAfter(ctx->exp(), " ");
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block());
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END("visitWhileStat");
    return nullptr;
}

// REPEAT block UNTIL exp;
antlrcpp::Any FormatVisitor::visitRepeatStat(LuaParser::RepeatStatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitRepeatStat");
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
    LOG_FUNCTION_END("visitRepeatStat");
    return nullptr;
}

// IF exp THEN block (ELSEIF exp THEN block)* (ELSE block)? END;
antlrcpp::Any FormatVisitor::visitIfStat(LuaParser::IfStatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitIfStat");
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
    LOG_FUNCTION_END("visitIfStat");
    return nullptr;
}

// FOR NAME EQL exp COMMA exp (COMMA exp)? DO block END;
antlrcpp::Any FormatVisitor::visitForStat(LuaParser::ForStatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitForStat");
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
    LOG_FUNCTION_END("visitForStat");
    return nullptr;
}

// FOR namelist IN explist DO block END;
antlrcpp::Any FormatVisitor::visitForInStat(LuaParser::ForInStatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitForInStat");
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
    LOG_FUNCTION_END("visitForInStat");
    return nullptr;
}

// FUNCTION funcname funcbody;
antlrcpp::Any FormatVisitor::visitFuncStat(LuaParser::FuncStatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitFuncStat");
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), " ");
    visitFuncname(ctx->funcname());
    cur_writer() << commentAfter(ctx->funcname(), "");
    visitFuncbody(ctx->funcbody());
    LOG_FUNCTION_END("visitFuncStat");
    return nullptr;
}

// LOCAL FUNCTION NAME funcbody;
antlrcpp::Any FormatVisitor::visitLocalFuncStat(LuaParser::LocalFuncStatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitLocalFuncStat");
    cur_writer() << ctx->LOCAL()->getText();
    cur_writer() << commentAfter(ctx->LOCAL(), " ");
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), " ");
    cur_writer() << ctx->NAME()->getText();
    cur_writer() << commentAfter(ctx->NAME(), "");
    visitFuncbody(ctx->funcbody());
    LOG_FUNCTION_END("visitLocalFuncStat");
    return nullptr;
}

// LOCAL namelist (EQL explist)? SEMI?;
antlrcpp::Any FormatVisitor::visitLocalVarDecl(LuaParser::LocalVarDeclContext* ctx) {
    LOG_FUNCTION_BEGIN("visitLocalVarDecl");
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
    LOG_FUNCTION_END("visitLocalVarDecl");
    return nullptr;
}

// RETURN explist? SEMI?;
antlrcpp::Any FormatVisitor::visitRetstat(LuaParser::RetstatContext* ctx) {
    LOG_FUNCTION_BEGIN("visitRetstat");
    if (chop_down_block_) {
        cur_writer() << indent();
    }
    cur_writer() << ctx->RETURN()->getText();
    if (ctx->explist() != NULL) {
        cur_writer() << commentAfter(ctx->RETURN(), " ");
        visitExplist(ctx->explist());
    }
    LOG_FUNCTION_END("visitRetstat");
    return nullptr;
}

// var (COMMA var)*;
antlrcpp::Any FormatVisitor::visitVarlist(LuaParser::VarlistContext* ctx) {
    LOG_FUNCTION_BEGIN("visitVarlist");
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
    LOG_FUNCTION_END("visitVarlist");
    return nullptr;
}

// NAME (COMMA NAME)*;
antlrcpp::Any FormatVisitor::visitNamelist(LuaParser::NamelistContext* ctx) {
    LOG_FUNCTION_BEGIN("visitNamelist");
    int n = ctx->COMMA().size();
    bool hasIncIndent = false;
    int firstParameterLength = ctx->NAME().front()->getText().size();
    int firstParameterIndent = 0;

    if (n > 0) firstParameterLength++;  // calc a ',' if exp > 1
    bool beyondLimit = cur_columns() + firstParameterLength > config_.column_limit();
    if (beyondLimit) {
        cur_writer() << "\n";
        incContinuationIndent();
        cur_writer() << indent();
        hasIncIndent = true;
    } else {
        firstParameterIndent = cur_columns() - indent_ - indentForAlign_;
    }
    cur_writer() << ctx->NAME().front()->getText();
    if (n > 0) {
        cur_writer() << commentAfter(ctx->NAME().front(), "");
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        bool beyondLimit = false;
        pushWriter();
        cur_writer() << commentAfter(ctx->COMMA()[i], " ");
        cur_writer() << ctx->NAME()[i + 1]->getText();
        int length = cur_writer().firstLineColumn();
        popWriter();
        if (i != n - 1) length++;  // calc a ',' if exp > 1
        beyondLimit = cur_columns() + length > config_.column_limit();

        if (beyondLimit) {
            if (hasIncIndent) {
                cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                cur_writer() << indent();
            } else {
                if (config_.align_parameter()) {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                    indent_ += firstParameterIndent;
                    cur_writer() << indent();
                    indent_ -= firstParameterIndent;
                } else {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], INC_CONTINUATION_INDENT);
                    cur_writer() << indent();
                    hasIncIndent = true;
                }
            }
            cur_writer() << ctx->NAME()[i + 1]->getText();
        } else {
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
            cur_writer() << ctx->NAME()[i + 1]->getText();
        }
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->NAME()[i + 1], "");
        }
    }
    if (hasIncIndent) {
        decContinuationIndent();
    }
    LOG_FUNCTION_END("visitNamelist");
    return nullptr;
}

// exp (COMMA exp)*;
antlrcpp::Any FormatVisitor::visitExplist(LuaParser::ExplistContext* ctx) {
    LOG_FUNCTION_BEGIN("visitExplist");
    int n = ctx->COMMA().size();
    pushWriterWithColumn();
    visitExp(ctx->exp().front());
    int expLength = cur_writer().firstLineColumn();
    int lines = cur_writer().lines();
    popWriter();
    if (n > 0) expLength++;  // calc a ',' if exp > 1
    bool beyondLimit = cur_columns() + expLength > config_.column_limit();
    // if lines > 1 and current column is already too long, then break
    // example:
    // var:foo(xxxx .. xxxx) -- no break
    // longlonglonglongvar:foo(
    //     xxxx .. xxxx) --  break

    if (lines > 1 && cur_columns() > config_.column_limit() / 2) {
        beyondLimit = true;
    }
    bool hasIncIndent = false;
    int firstArgsIndent = 0;
    if (beyondLimit) {
        if (cur_writer().isLastCharWhiteSpace()) {
            cur_writer().ss().seekp(-1, ios::end);
        }
        cur_writer() << "\n";
        incContinuationIndent();
        cur_writer() << indentWithAlign();
        hasIncIndent = true;
        visitExp(ctx->exp().front());
    } else {
        firstArgsIndent = cur_columns() - indent_ - indentForAlign_;
        incIndentForAlign(firstArgsIndent);
        visitExp(ctx->exp().front());
        decIndentForAlign(firstArgsIndent);
    }
    if (n > 0) {
        cur_writer() << commentAfter(ctx->exp().front(), "");
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        bool beyondLimit = false;
        pushWriter();
        cur_writer() << commentAfter(ctx->COMMA()[i], " ");
        visitExp(ctx->exp()[i + 1]);
        int expLength = cur_writer().firstLineColumn();
        popWriter();
        if (i != n - 1) expLength++;  // calc a ',' if exp > 1
        beyondLimit = cur_columns() + expLength > config_.column_limit();
        if (beyondLimit) {
            bool hasIncIndentForAlign = false;
            if (hasIncIndent) {
                cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                cur_writer() << indentWithAlign();
            } else {
                if (config_.align_args()) {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                    incIndentForAlign(firstArgsIndent);
                    cur_writer() << indentWithAlign();
                    hasIncIndentForAlign = true;
                } else {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], INC_CONTINUATION_INDENT);
                    cur_writer() << indentWithAlign();
                    hasIncIndent = true;
                }
            }
            visitExp(ctx->exp()[i + 1]);
            if (hasIncIndentForAlign) {
                decIndentForAlign(firstArgsIndent);
            }
        } else {
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
            visitExp(ctx->exp()[i + 1]);
        }
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->exp()[i + 1], "");
        }
    }
    if (hasIncIndent) {
        decContinuationIndent();
    }
    LOG_FUNCTION_END("visitExplist");
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
    LOG_FUNCTION_BEGIN("visitExp");
    if (ctx->linkOperator() != NULL) {
        visitExp(ctx->exp()[0]);
        bool hasIncIndent = false;
        if (config_.break_after_operator()) {
            cur_writer() << commentAfter(ctx->exp()[0], " ");
            cur_writer() << ctx->linkOperator()->getText();
            bool beyondLimit = false;
            pushWriter();
            visitExp(ctx->exp()[1]);
            int length = cur_writer().firstLineColumn();
            length++;  // calc the white space after operator
            popWriter();
            beyondLimit = cur_columns() + length > config_.column_limit();
            if (beyondLimit) {
                cur_writer() << commentAfterNewLine(ctx->linkOperator(), INC_CONTINUATION_INDENT);
                cur_writer() << indentWithAlign();
                hasIncIndent = true;
            } else {
                cur_writer() << commentAfter(ctx->linkOperator(), " ");
            }
            visitExp(ctx->exp()[1]);
        } else {
            bool beyondLimit = false;
            pushWriter();
            cur_writer() << ctx->linkOperator()->getText();
            cur_writer() << commentAfter(ctx->linkOperator(), " ");
            visitExp(ctx->exp()[1]);
            int length = cur_writer().firstLineColumn();
            popWriter();
            beyondLimit = cur_columns() + length > config_.column_limit();
            if (beyondLimit) {
                cur_writer() << commentAfterNewLine(ctx->exp()[0], INC_CONTINUATION_INDENT);
                cur_writer() << indentWithAlign();
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
    LOG_FUNCTION_END("visitExp");
    return nullptr;
}

// varOrExp nameAndArgs*;
antlrcpp::Any FormatVisitor::visitPrefixexp(LuaParser::PrefixexpContext* ctx) {
    LOG_FUNCTION_BEGIN("visitPrefixexp");
    chainedMethodCallHasIncIndent_.push_back(false);
    chainedMethodCallIsFirst_.push_back(false);
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
    chainedMethodCallIsFirst_.pop_back();
    LOG_FUNCTION_END("visitPrefixexp");
    return nullptr;
}

// var | LP exp RP;
antlrcpp::Any FormatVisitor::visitVarOrExp(LuaParser::VarOrExpContext* ctx) {
    LOG_FUNCTION_BEGIN("visitVarOrExp");
    if (ctx->exp() != NULL) {
        cur_writer() << ctx->LP()->getText();
        cur_writer() << commentAfter(ctx->LP(), "");
        visitExp(ctx->exp());
        cur_writer() << commentAfter(ctx->exp(), "");
        cur_writer() << ctx->RP()->getText();

    } else {
        visitVar(ctx->var());
    }
    LOG_FUNCTION_END("visitVarOrExp");
    return nullptr;
}

// (NAME | LP exp RP varSuffix) varSuffix*;
antlrcpp::Any FormatVisitor::visitVar(LuaParser::VarContext* ctx) {
    LOG_FUNCTION_BEGIN("visitVar");
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
    LOG_FUNCTION_END("visitVar");
    return nullptr;
}

// nameAndArgs* (LSB exp RSB | DOT NAME);
antlrcpp::Any FormatVisitor::visitVarSuffix(LuaParser::VarSuffixContext* ctx) {
    LOG_FUNCTION_BEGIN("visitVarSuffix");
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
        LuaParser::VarContext* varCtx = dynamic_cast<LuaParser::VarContext*>(ctx->parent);
        const vector<LuaParser::VarSuffixContext*>& arr = varCtx->varSuffix();
        int index = find(arr.begin(), arr.end(), ctx) - arr.begin();
        if (index != 0) {
            pushWriterWithColumn();
            cur_writer() << ctx->DOT()->getText();
            cur_writer() << commentAfter(ctx->DOT(), "");
            cur_writer() << ctx->NAME()->getText();
            visitNextNameAndArgs(ctx);
            int length = cur_writer().firstLineColumn();
            popWriter();
            bool beyondLimit = cur_columns() + length > config_.column_limit();
            if (beyondLimit) {
                cur_writer() << "\n";
                if (chainedMethodCallHasIncIndent_.empty()) {
                    // chainedMethodCallHasIncIndent_ is empty when visit var in varlist
                    cur_writer() << indentWithAlign();
                } else {
                    if (chainedMethodCallHasIncIndent_.back()) {
                        cur_writer() << indentWithAlign();
                    } else {
                        incContinuationIndent();
                        cur_writer() << indentWithAlign();
                        chainedMethodCallHasIncIndent_[chainedMethodCallHasIncIndent_.size() - 1] = true;
                    }
                }
            }
        }
        cur_writer() << ctx->DOT()->getText();
        cur_writer() << commentAfter(ctx->DOT(), "");
        cur_writer() << ctx->NAME()->getText();
    }
    LOG_FUNCTION_END("visitVarSuffix");
    return nullptr;
}

// visit next NameAndArgsContext of VarSuffixContext
void FormatVisitor::visitNextNameAndArgs(LuaParser::VarSuffixContext* ctx) {
    // find args of NAME
    LuaParser::VarContext* varCtx = dynamic_cast<LuaParser::VarContext*>(ctx->parent);
    const vector<LuaParser::VarSuffixContext*>& arr = varCtx->varSuffix();
    int index = find(arr.begin(), arr.end(), ctx) - arr.begin();
    if (index + 1 < arr.size()) {
        for (auto na : arr[index + 1]->nameAndArgs()) {
            // if next NameAndArgs's COLON is not null
            // then formatter can place a break before COLON.
            // So do not need to calc column length for current NAME, break the loop.
            // example:
            // self.xxx.yyy:foo()
            // the 'xxx' and 'yyy' is a 'Variable', and 'foo' is a 'Method'
            // It better break before 'foo' than break before 'yyy'
            if (na->COLON() != nullptr) break;
            visitNameAndArgs(na);
            if (na != arr[index + 1]->nameAndArgs().back()) {
                cur_writer() << commentAfter(na, "");
            }
        }
    } else {
        LuaParser::VarOrExpContext* varOrExpCtx = dynamic_cast<LuaParser::VarOrExpContext*>(varCtx->parent);
        // parent maybe VarlistContext
        if (varOrExpCtx != nullptr) {
            tree::ParseTree* parent = varOrExpCtx->parent;

            LuaParser::PrefixexpContext* peCtx = dynamic_cast<LuaParser::PrefixexpContext*>(parent);
            if (peCtx != nullptr) {
                for (auto na : peCtx->nameAndArgs()) {
                    if (na->COLON() != nullptr) break;
                    visitNameAndArgs(na);
                    if (na != peCtx->nameAndArgs().back()) {
                        cur_writer() << commentAfter(na, "");
                    }
                }
            }

            LuaParser::FunctioncallContext* fcCtx = dynamic_cast<LuaParser::FunctioncallContext*>(parent);
            if (fcCtx != nullptr) {
                for (auto na : fcCtx->nameAndArgs()) {
                    if (na->COLON() != nullptr) break;
                    visitNameAndArgs(na);
                    if (na != fcCtx->nameAndArgs().back()) {
                        cur_writer() << commentAfter(na, "");
                    }
                }
            }
        }
    }
}

// (COLON NAME)? args;
antlrcpp::Any FormatVisitor::visitNameAndArgs(LuaParser::NameAndArgsContext* ctx) {
    LOG_FUNCTION_BEGIN("visitNameAndArgs");
    if (ctx->COLON() != NULL) {
        bool beyondLimit = false;
        LuaParser::StringContext* stringContext = ctx->args()->string();
        bool needWhiteSpace = stringContext != NULL;
        if (!chainedMethodCallIsFirst_.back()) {
            chainedMethodCallIsFirst_[chainedMethodCallIsFirst_.size() - 1] = true;
        } else {
            pushWriterWithColumn();
            cur_writer() << ctx->COLON()->getText();
            cur_writer() << commentAfter(ctx->COLON(), "");
            cur_writer() << ctx->NAME()->getText();
            cur_writer() << commentAfter(ctx->NAME(), needWhiteSpace ? " " : "");
            visitArgs(ctx->args());
            int length = cur_writer().firstLineColumn();
            popWriter();
            beyondLimit = cur_columns() + length > config_.column_limit();
        }
        if (beyondLimit) {
            cur_writer() << "\n";
            if (chainedMethodCallHasIncIndent_.back()) {
                cur_writer() << indentWithAlign();
            } else {
                incContinuationIndent();
                cur_writer() << indentWithAlign();
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
    LOG_FUNCTION_END("visitNameAndArgs");
    return nullptr;
}

// LP explist? RP | tableconstructor | string;
antlrcpp::Any FormatVisitor::visitArgs(LuaParser::ArgsContext* ctx) {
    LOG_FUNCTION_BEGIN("visitArgs");
    if (ctx->LP() != NULL) {
        cur_writer() << ctx->LP()->getText();
        if (ctx->explist() != NULL) {
            bool beyondLimit = false;
            bool breakAfterLp = false;
            pushWriterWithColumn();
            // if (!config_.break_before_functioncall_rp()) cur_writer() << "-";
            cur_writer() << commentAfter(ctx->LP(), "");
            visitExplist(ctx->explist());
            cur_writer() << commentAfter(ctx->explist(), "");
            int length = cur_writer().firstLineColumn();
            int lines = cur_writer().lines();
            if (!config_.break_before_functioncall_rp()) length++;
            popWriter();
            beyondLimit = cur_columns() + length > config_.column_limit();
            if (beyondLimit || lines > 1) breakAfterLp = config_.break_after_functioncall_lp();
            bool hasIncIndent = false;
            if (breakAfterLp) {
                hasIncIndent = true;
                cur_writer() << commentAfterNewLine(ctx->LP(), INC_CONTINUATION_INDENT);
                cur_writer() << indentWithAlign();
            } else {
                cur_writer() << commentAfter(ctx->LP(), "");
            }
            visitExplist(ctx->explist());
            if (hasIncIndent) {
                if (config_.break_before_functioncall_rp()) {
                    cur_writer() << commentAfterNewLine(ctx->explist(), DEC_CONTINUATION_INDENT);
                    cur_writer() << indentWithAlign();
                } else {
                    decContinuationIndent();
                    cur_writer() << commentAfter(ctx->explist(), "");
                }
            } else {
                cur_writer() << commentAfter(ctx->explist(), "");
            }
            cur_writer() << ctx->RP()->getText();
        } else {
            cur_writer() << commentAfter(ctx->LP(), "");
            cur_writer() << ctx->RP()->getText();
        }
    } else {
        visitChildren(ctx);
    }
    LOG_FUNCTION_END("visitArgs");
    return nullptr;
}

// FUNCTION funcbody;
antlrcpp::Any FormatVisitor::visitFunctiondef(LuaParser::FunctiondefContext* ctx) {
    LOG_FUNCTION_BEGIN("visitFunctiondef");
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), "");
    // disable indentForAlign_ in function body
    int temp = indentForAlign_;
    indentForAlign_ = 0;
    visitFuncbody(ctx->funcbody());
    indentForAlign_ = temp;
    LOG_FUNCTION_END("visitFunctiondef");
    return nullptr;
}

// LP parlist? RP block END
antlrcpp::Any FormatVisitor::visitFuncbody(LuaParser::FuncbodyContext* ctx) {
    LOG_FUNCTION_BEGIN("visitFuncbody");
    cur_writer() << ctx->LP()->getText();
    if (ctx->parlist() != NULL) {
        bool breakAfterLp = false;
        bool beyondLimit = false;
        pushWriter();
        cur_writer() << commentAfter(ctx->LP(), "");
        visitParlist(ctx->parlist());
        cur_writer() << commentAfter(ctx->parlist(), "");
        cur_writer() << ctx->RP()->getText();
        int length = cur_writer().firstLineColumn();
        popWriter();
        beyondLimit = cur_columns() + length > config_.column_limit();
        if (beyondLimit) {
            breakAfterLp = config_.break_after_functiondef_lp();
        }
        if (breakAfterLp) {
            cur_writer() << commentAfterNewLine(ctx->LP(), INC_CONTINUATION_INDENT);
            cur_writer() << indent();
        } else {
            cur_writer() << commentAfter(ctx->LP(), "");
        }

        visitParlist(ctx->parlist());

        if (config_.break_before_functiondef_rp()) {
            if (breakAfterLp) {
                cur_writer() << commentAfterNewLine(ctx->parlist(), DEC_CONTINUATION_INDENT);
                cur_writer() << indent();
            } else {
                cur_writer() << commentAfter(ctx->parlist(), "");
            }
        } else {
            if (breakAfterLp) decContinuationIndent();
            cur_writer() << commentAfter(ctx->parlist(), "");
        }
    } else {
        cur_writer() << commentAfter(ctx->LP(), "");
    }
    cur_writer() << ctx->RP()->getText();
    visitBlockAndComment(ctx->RP(), ctx->block());
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END("visitFuncbody");
    return nullptr;
}

// namelist (COMMA ELLIPSIS)? | ELLIPSIS
antlrcpp::Any FormatVisitor::visitParlist(LuaParser::ParlistContext* ctx) {
    LOG_FUNCTION_BEGIN("visitParlist");
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
    LOG_FUNCTION_END("visitParlist");
    return nullptr;
}

// LB fieldlist? RB;
antlrcpp::Any FormatVisitor::visitTableconstructor(LuaParser::TableconstructorContext* ctx) {
    LOG_FUNCTION_BEGIN("visitTableconstructor");
    cur_writer() << ctx->LB()->getText();
    // disable indentForAlign_ in table
    int temp = indentForAlign_;
    indentForAlign_ = 0;
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
        bool beyondLimit = fastTestColumnLimit(ctx->fieldlist());
        if (!beyondLimit) {
            pushWriter();
            cur_writer() << commentAfter(ctx->LB(), "");
            chop_down_table_ = false;
            visitFieldlist(ctx->fieldlist());
            chop_down_table_ = temp;
            cur_writer() << commentAfter(ctx->fieldlist(), "");
            cur_writer() << ctx->RB()->getText();
            int length = cur_writer().firstLineColumn();
            int lines = cur_writer().lines();
            popWriter();
            beyondLimit = cur_columns() + length > config_.column_limit() || lines > 1;
        }
        bool breakAfterLb = false;
        if (beyondLimit) {
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
        bool needExtraFieldSep = config_.extra_sep_at_table_end() && ctx->fieldlist()->field().size() > 0;
        if (chopDown) {
            if (needExtraFieldSep) cur_writer() << config_.table_sep();
            cur_writer() << commentAfterNewLine(ctx->fieldlist(), DEC_INDENT);
            cur_writer() << indent();
        } else {
            if (breakAfterLb) {
                if (config_.break_before_table_rb()) {
                    if (needExtraFieldSep) cur_writer() << config_.table_sep();
                    cur_writer() << commentAfterNewLine(ctx->fieldlist(), DEC_INDENT);
                    cur_writer() << indent();
                } else {
                    decIndent();
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
    indentForAlign_ = temp;
    LOG_FUNCTION_END("visitTableconstructor");
    return nullptr;
}

// field (fieldsep field)* fieldsep?;
antlrcpp::Any FormatVisitor::visitFieldlist(LuaParser::FieldlistContext* ctx) {
    LOG_FUNCTION_BEGIN("visitFieldlist");
    int n = ctx->field().size();
    int sn = ctx->fieldsep().size();
    if (config_.align_table_field()) {
        firstTableFieldColumn_.push_back(cur_columns());
    }
    bool hasIncIndent = cur_writer().isLastCharWhiteSpace();
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
        bool beyondLimit = false;
        pushWriter();
        cur_writer() << commentAfter(ctx->fieldsep()[i - 1], " ");
        visitField(ctx->field()[i]);
        int length = cur_writer().firstLineColumn();
        popWriter();
        if (i != n - 1 || config_.extra_sep_at_table_end()) length++;  // calc a ',' if exp > 1
        beyondLimit = cur_columns() + length > config_.column_limit();
        if (beyondLimit) {
            if (config_.align_table_field()) {
                cur_writer() << commentAfterNewLine(ctx->fieldsep()[i - 1], NONE_INDENT);
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
                    hasIncIndent = true;
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
    if (config_.align_table_field()) {
        firstTableFieldColumn_.pop_back();
    }
    if (customIncIndent) {
        decIndent();
    }
    LOG_FUNCTION_END("visitFieldlist");
    return nullptr;
}

// LSB exp RSB EQL exp | NAME EQL exp | exp;
antlrcpp::Any FormatVisitor::visitField(LuaParser::FieldContext* ctx) {
    LOG_FUNCTION_BEGIN("visitField");
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
    LOG_FUNCTION_END("visitField");
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
    if (fastTestColumnLimit(ctx)) return false;
    pushWriter();
    visitBlock(ctx);
    int length = cur_writer().firstLineColumn();
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

bool FormatVisitor::isBlockEmpty(LuaParser::BlockContext* ctx) {
    return ctx->stat().size() == 0 && ctx->retstat() == NULL;
}

void FormatVisitor::visitBlockAndComment(tree::ParseTree* previousNode, LuaParser::BlockContext* ctx) {
    LOG_FUNCTION_BEGIN("visitBlockAndComment");
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
    LOG_FUNCTION_END("visitBlockAndComment");
}

string FormatVisitor::indent() {
    stringstream ss;
    for (int i = 0; i < indent_; i++) {
        if (config_.use_tab()) {
            ss << "\t";
        } else {
            ss << " ";
        }
    }
    return ss.str();
}

string FormatVisitor::indentWithAlign() {
    stringstream ss;
    for (int i = 0; i < indent_ + indentForAlign_; i++) {
        if (config_.use_tab()) {
            ss << "\t";
        } else {
            ss << " ";
        }
    }
    return ss.str();
}

void FormatVisitor::incIndent() {
    LOG_FUNCTION_BEGIN("incIndent");
    indent_ += config_.indent_width();
    LOG_FUNCTION_END("incIndent");
}

void FormatVisitor::decIndent() {
    LOG_FUNCTION_BEGIN("decIndent");
    indent_ -= config_.indent_width();
    LOG_FUNCTION_END("decIndent");
}

void FormatVisitor::incContinuationIndent() {
    LOG_FUNCTION_BEGIN("incContinuationIndent");
    indent_ += config_.continuation_indent_width();
    LOG_FUNCTION_END("incContinuationIndent");
}

void FormatVisitor::decContinuationIndent() {
    LOG_FUNCTION_BEGIN("decContinuationIndent");
    indent_ -= config_.continuation_indent_width();
    LOG_FUNCTION_END("decContinuationIndent");
}

void FormatVisitor::incIndentForAlign(int indent) {
    LOG_FUNCTION_BEGIN("incIndentForAlign");
    indentForAlign_ += indent;
    LOG_FUNCTION_END("incIndentForAlign");
}

void FormatVisitor::decIndentForAlign(int indent) {
    LOG_FUNCTION_BEGIN("decIndentForAlign");
    indentForAlign_ -= indent;
    LOG_FUNCTION_END("decIndentForAlign");
}