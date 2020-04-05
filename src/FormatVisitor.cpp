#include "FormatVisitor.h"

#include <iostream>
#include <regex>

#include "LuaLexer.h"

using namespace std;
using namespace antlr4;

// #define LOG_FLAG

#ifdef LOG_FLAG
#define LOGVAR()                                         \
    for (int i = 0; i < logIndentSize; i++) cout << " "; \
    cout << "\"" << __func__ << "\" = ";                 \
    cout << __func__ << endl
#define LOG()                                            \
    for (int i = 0; i < logIndentSize; i++) cout << " "; \
    cout << __func__ << endl
static int logIndentSize = 0;
#define LOG_FUNCTION_BEGIN()                             \
    logIndentSize++;                                     \
    for (int i = 0; i < logIndentSize; i++) cout << " "; \
    cout << __func__ << " begin.." << endl
#define LOG_FUNCTION_END()                               \
    for (int i = 0; i < logIndentSize; i++) cout << " "; \
    logIndentSize--;                                     \
    cout << __func__ << " end.." << endl
#else
#define LOG()
#define LOGVAR()
#define LOG_FUNCTION_BEGIN()
#define LOG_FUNCTION_END()
#endif

static bool isBlankChar(const char c) { return c == ' ' || c == '\t' || c == '\u000C'; }

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
    for (unsigned int i = l + 1; i < tokens_.size(); i++) {
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
    for (unsigned int i = l + 1; i < tokens_.size(); i++) {
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
            auto ln = token->getText().find("\n");
            auto rn = token->getText().rfind("\n");
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
        if (columns > config_.get<int>("column_limit")) {
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
    LOG_FUNCTION_BEGIN();
    pushWriter();
    cur_writer() << commentAfter(NULL, "");
    bool temp = chop_down_block_;
    chop_down_block_ = true;
    visitBlock(ctx->block());
    chop_down_block_ = temp;
    string comment = commentAfterNewLine(ctx->block(), NONE_INDENT);

    cur_writer() << comment;
    string ret = cur_writer().str();
    if (*ret.rbegin() != '\n') ret += '\n';
    popWriter();
    LOG_FUNCTION_END();
    return ret;
}

// stat* retstat?
antlrcpp::Any FormatVisitor::visitBlock(LuaParser::BlockContext* ctx) {
    LOG_FUNCTION_BEGIN();
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
    LOG_FUNCTION_END();
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitStat(LuaParser::StatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (chop_down_block_) {
        cur_writer() << indent();
    }
    visitChildren(ctx);
    LOG_FUNCTION_END();
    return nullptr;
}

// varlist EQL explist;
antlrcpp::Any FormatVisitor::visitVarDecl(LuaParser::VarDeclContext* ctx) {
    LOG_FUNCTION_BEGIN();
    visitVarlist(ctx->varlist());
    cur_writer() << commentAfter(ctx->varlist(), " ");
    cur_writer() << ctx->EQL()->getText();
    cur_writer() << commentAfter(ctx->EQL(), " ");
    visitExplist(ctx->explist());
    if (ctx->SEMI() != NULL) {
        cur_writer() << commentAfter(ctx->explist(), "");
        cur_writer() << ctx->SEMI()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// Builds first argument whitespace in spaces.
// local a = f<WS>{1, 2, 3} or f<WS>"a"
string FormatVisitor::buildFirstArgumentWs(vector<LuaParser::NameAndArgsContext*> v) {
    bool needWhiteSpace = false;

    if (v.size() > 0) {
        LuaParser::NameAndArgsContext* ctx = v.front();
        if (ctx->COLON() == NULL && ctx->args()->LP() == NULL) {
            needWhiteSpace = true;
        }
    }

    // Build whitespace
    string ws = "";
    if (needWhiteSpace) {
        int sz = config_.get<int>("spaces_before_call");
        while (sz--) {
            ws.append(" ");
        }
    }

    return ws;
}

// Builds arguments whitespaces in spaces.
// local a = f"a"<WS>"b"<WS>"c"...
void FormatVisitor::buildArguments(vector<LuaParser::NameAndArgsContext*> v) {
    for (size_t i = 0; i < v.size(); i++) {
        auto ctx = v[i];
        string ws = "";

        // Write out the argument
        visitNameAndArgs(ctx);

        if (ctx == v.back())  // Last element, bail out
            break;

        auto nctx = v[i + 1];
        if (nctx->COLON() == NULL && (nctx->args()->string() || nctx->args()->tableconstructor())) ws = " ";

        cur_writer() << commentAfter(ctx, ws);
    }
}

// varOrExp nameAndArgs+;
antlrcpp::Any FormatVisitor::visitFunctioncall(LuaParser::FunctioncallContext* ctx) {
    LOG_FUNCTION_BEGIN();
    chainedMethodCallHasIncIndent_.push_back(false);
    chainedMethodCallIsFirst_.push_back(false);
    visitVarOrExp(ctx->varOrExp());

    string ws = buildFirstArgumentWs(ctx->nameAndArgs());
    cur_writer() << commentAfter(ctx->varOrExp(), ws);

    buildArguments(ctx->nameAndArgs());

    if (ctx->SEMI() != NULL) {
        cur_writer() << commentAfter(ctx->nameAndArgs().back(), "");
        cur_writer() << ctx->SEMI()->getText();
    }
    if (chainedMethodCallHasIncIndent_.back()) {
        decContinuationIndent();
    }
    chainedMethodCallHasIncIndent_.pop_back();
    chainedMethodCallIsFirst_.pop_back();
    LOG_FUNCTION_END();
    return nullptr;
}

// GOTO NAME;
antlrcpp::Any FormatVisitor::visitGotoStat(LuaParser::GotoStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->GOTO()->getText();
    cur_writer() << commentAfter(ctx->GOTO(), " ");
    cur_writer() << ctx->NAME()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// DO block END;
antlrcpp::Any FormatVisitor::visitDoStat(LuaParser::DoStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// WHILE exp DO block END;
antlrcpp::Any FormatVisitor::visitWhileStat(LuaParser::WhileStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->WHILE()->getText();
    cur_writer() << commentAfter(ctx->WHILE(), " ");
    visitExp(ctx->exp());
    cur_writer() << commentAfter(ctx->exp(), " ");
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// REPEAT block UNTIL exp;
antlrcpp::Any FormatVisitor::visitRepeatStat(LuaParser::RepeatStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->REPEAT()->getText();
    visitBlockAndComment(ctx->REPEAT(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->UNTIL()->getText();
    cur_writer() << commentAfter(ctx->UNTIL(), " ");
    visitExp(ctx->exp());
    if (ctx->SEMI() != NULL) {
        cur_writer() << commentAfter(ctx->exp(), "");
        cur_writer() << ctx->SEMI()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// IF exp THEN block (ELSEIF exp THEN block)* (ELSE block)? END;
antlrcpp::Any FormatVisitor::visitIfStat(LuaParser::IfStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    // keep if statement one line only when there is no elseif or else statement.
    cur_writer() << ctx->IF()->getText();
    cur_writer() << commentAfter(ctx->IF(), " ");
    visitExp(ctx->exp().front());
    cur_writer() << commentAfter(ctx->exp().front(), " ");
    cur_writer() << ctx->THEN().front()->getText();
    if (ctx->ELSEIF().size() == 0 && ctx->ELSE() == NULL) {
        if (needKeepBlockOneLine(ctx->THEN().front(), ctx->block().front(), CONTROL_BLOCK)) {
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
    LOG_FUNCTION_END();
    return nullptr;
}

// FOR NAME EQL exp COMMA exp (COMMA exp)? DO block END;
antlrcpp::Any FormatVisitor::visitForStat(LuaParser::ForStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
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
    visitBlockAndComment(ctx->DO(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// FOR namelist IN explist DO block END;
antlrcpp::Any FormatVisitor::visitForInStat(LuaParser::ForInStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->FOR()->getText();
    cur_writer() << commentAfter(ctx->FOR(), " ");
    visitNamelist(ctx->namelist());
    cur_writer() << commentAfter(ctx->namelist(), " ");
    cur_writer() << ctx->IN()->getText();
    cur_writer() << commentAfter(ctx->IN(), " ");
    visitExplist(ctx->explist());
    cur_writer() << commentAfter(ctx->explist(), " ");
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// FUNCTION funcname funcbody;
antlrcpp::Any FormatVisitor::visitFuncStat(LuaParser::FuncStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), " ");
    visitFuncname(ctx->funcname());
    cur_writer() << commentAfter(ctx->funcname(), "");
    visitFuncbody(ctx->funcbody());
    LOG_FUNCTION_END();
    return nullptr;
}

// LOCAL FUNCTION NAME funcbody;
antlrcpp::Any FormatVisitor::visitLocalFuncStat(LuaParser::LocalFuncStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->LOCAL()->getText();
    cur_writer() << commentAfter(ctx->LOCAL(), " ");
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), " ");
    cur_writer() << ctx->NAME()->getText();
    cur_writer() << commentAfter(ctx->NAME(), "");
    visitFuncbody(ctx->funcbody());
    LOG_FUNCTION_END();
    return nullptr;
}

// LOCAL namelist (EQL explist)? SEMI?;
antlrcpp::Any FormatVisitor::visitLocalVarDecl(LuaParser::LocalVarDeclContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->LOCAL()->getText();
    cur_writer() << commentAfter(ctx->LOCAL(), " ");
    visitAttnamelist(ctx->attnamelist());
    if (ctx->EQL() != NULL) {
        cur_writer() << commentAfter(ctx->attnamelist(), " ");
        cur_writer() << ctx->EQL()->getText();
        cur_writer() << commentAfter(ctx->EQL(), " ");
        visitExplist(ctx->explist());
    }
    if (ctx->SEMI() != NULL) {
        if (ctx->EQL() == NULL) {
            cur_writer() << commentAfter(ctx->attnamelist(), "");
        } else {
            cur_writer() << commentAfter(ctx->explist(), "");
        }
        cur_writer() << ctx->SEMI()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// RETURN explist? SEMI?;
antlrcpp::Any FormatVisitor::visitRetstat(LuaParser::RetstatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (chop_down_block_) {
        cur_writer() << indent();
    }
    cur_writer() << ctx->RETURN()->getText();
    if (ctx->explist() != NULL) {
        cur_writer() << commentAfter(ctx->RETURN(), " ");
        visitExplist(ctx->explist());
    }
    if (ctx->SEMI() != NULL) {
        cur_writer() << ctx->SEMI()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// var (COMMA var)*;
antlrcpp::Any FormatVisitor::visitVarlist(LuaParser::VarlistContext* ctx) {
    LOG_FUNCTION_BEGIN();
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
    LOG_FUNCTION_END();
    return nullptr;
}

// NAME (COMMA NAME)*;
antlrcpp::Any FormatVisitor::visitNamelist(LuaParser::NamelistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    int n = ctx->COMMA().size();
    bool hasIncIndent = false;
    int firstParameterLength = ctx->NAME().front()->getText().size();
    int firstParameterIndent = 0;

    if (n > 0) firstParameterLength++;  // calc a ',' if exp > 1
    bool beyondLimit = cur_columns() + firstParameterLength > config_.get<int>("column_limit");
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
    bool chopDownParameter = false;
    if (config_.get<bool>("chop_down_parameter")) {
        if (fastTestColumnLimit(ctx)) {
            chopDownParameter = true;
        } else {
            pushWriter();
            for (int i = 0; i < n; i++) {
                cur_writer() << ctx->COMMA()[i]->getText();
                cur_writer() << commentAfter(ctx->COMMA()[i], " ");
                cur_writer() << ctx->NAME()[i + 1]->getText();
                if (i != n - 1) {
                    cur_writer() << commentAfter(ctx->NAME()[i + 1], "");
                }
            }
            int length = cur_writer().firstLineColumn();
            int lines = cur_writer().lines();
            popWriter();
            chopDownParameter = lines > 1 || cur_columns() + length > config_.get<int>("column_limit");
        }
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        bool beyondLimit = false;
        if (chopDownParameter) {
            beyondLimit = true;
        } else {
            pushWriter();
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
            cur_writer() << ctx->NAME()[i + 1]->getText();
            int length = cur_writer().firstLineColumn();
            popWriter();
            if (i != n - 1) length++;  // calc a ',' if exp > 1
            beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
        }
        if (beyondLimit) {
            if (hasIncIndent) {
                cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                cur_writer() << indent();
            } else {
                if (config_.get<bool>("align_parameter")) {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                    indent_ += firstParameterIndent;
                    cur_writer() << indent();
                    indent_ -= firstParameterIndent;
                } else {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                    cur_writer() << indent();
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
    LOG_FUNCTION_END();
    return nullptr;
}

// NAME attrib (COMMA NAME attrib)*;
antlrcpp::Any FormatVisitor::visitAttnamelist(LuaParser::AttnamelistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    int n = ctx->COMMA().size();
    bool hasIncIndent = false;
    int firstParameterLength = ctx->NAME().front()->getText().size() + ctx->attrib().front()->getText().size();
    int firstParameterIndent = 0;

    if (n > 0) firstParameterLength++;  // calc a ',' if exp > 1
    bool beyondLimit = cur_columns() + firstParameterLength > config_.get<int>("column_limit");
    if (beyondLimit) {
        cur_writer() << "\n";
        incContinuationIndent();
        cur_writer() << indent();
        hasIncIndent = true;
    } else {
        firstParameterIndent = cur_columns() - indent_ - indentForAlign_;
    }
    cur_writer() << ctx->NAME().front()->getText();
    string attrib = ctx->attrib().front()->getText();
    // judge attrib size, avoid print comment twice
    if (attrib.size() > 0) {
        cur_writer() << commentAfter(ctx->NAME().front(), "");
        cur_writer() << attrib;
        if (n > 0) {
            cur_writer() << commentAfter(ctx->attrib().front(), "");
        }
    } else {
        if (n > 0) {
            cur_writer() << commentAfter(ctx->NAME().front(), "");
        }
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        bool beyondLimit = false;
        pushWriter();
        cur_writer() << commentAfter(ctx->COMMA()[i], " ");
        cur_writer() << ctx->NAME()[i + 1]->getText();
        cur_writer() << ctx->attrib()[i + 1]->getText();
        int length = cur_writer().firstLineColumn();
        popWriter();
        if (i != n - 1) length++;  // calc a ',' if exp > 1
        beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
        if (beyondLimit) {
            if (hasIncIndent) {
                cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                cur_writer() << indent();
            } else {
                if (config_.get<bool>("align_parameter")) {
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
            cur_writer() << ctx->attrib()[i + 1]->getText();
        } else {
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
            cur_writer() << ctx->NAME()[i + 1]->getText();
            cur_writer() << ctx->attrib()[i + 1]->getText();
        }
        if (i != n - 1) {
            if (ctx->attrib()[i + 1]->getText().size() > 0) {
                cur_writer() << commentAfter(ctx->attrib()[i + 1], "");
            } else {
                cur_writer() << commentAfter(ctx->NAME()[i + 1], "");
            }
        }
    }
    if (hasIncIndent) {
        decContinuationIndent();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// (LT NAME GT)?;
antlrcpp::Any FormatVisitor::visitAttrib(LuaParser::AttribContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (ctx->NAME() != NULL) {
        cur_writer() << ctx->LT()->getText();
        cur_writer() << ctx->NAME()->getText();
        cur_writer() << ctx->GT()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// exp (COMMA exp)*;
antlrcpp::Any FormatVisitor::visitExplist(LuaParser::ExplistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    int n = ctx->COMMA().size();
    pushWriterWithColumn();
    visitExp(ctx->exp().front());
    int expLength = cur_writer().firstLineColumn();
    int lines = cur_writer().lines();
    popWriter();
    if (n > 0) expLength++;  // calc a ',' if exp > 1
    bool beyondLimit = cur_columns() + expLength > config_.get<int>("column_limit");
    // if lines > 1 and current column is already too long, then break
    // example:
    // var:foo(xxxx .. xxxx) -- no break
    // longlonglonglongvar:foo(
    //     xxxx .. xxxx) --  break

    if (lines > 1 && cur_columns() > config_.get<int>("column_limit") / 2) {
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
        int lines = cur_writer().lines();
        popWriter();
        if (i != n - 1) expLength++;  // calc a ',' if exp > 1
        beyondLimit = cur_columns() + expLength > config_.get<int>("column_limit");
        if (beyondLimit && lines == 1) {
            // lines is 1 means expression is enough short. But with current columns,
            // it beyonds column limit, need a line break here.
            // But if after a line break, the expression is still beyond column limit.
            // Then don't let it break the line at the beginning.
            // example:
            // longlonglongvar.longlonglongmethod(arg11111, function() print("11111111111111111111111111111111") end)
            //
            // This function call is beyond column limit when visit the second args.
            // so if we break after ',' , it become:
            // longlonglongvar.longlonglongmethod(arg11111,
            //                                    function() print("11111111111111111111111111111111") end)
            //
            // The second args is still too long, and need another break:
            // longlonglongvar.longlonglongmethod(arg11111,
            //                                    function()
            //     print("11111111111111111111111111111111")
            // end)
            //
            // The first break (before 'function()') doesn't seem to make sense.
            // So determine if this expression is still beyond column limit after break.
            // If true, determine the column again with current columns.
            if (indentWithAlign().size() + expLength + firstArgsIndent > config_.get<unsigned int>("column_limit")) {
                pushWriterWithColumn();
                cur_writer() << commentAfter(ctx->COMMA()[i], " ");
                visitExp(ctx->exp()[i + 1]);
                int expLength = cur_writer().firstLineColumn();
                popWriter();
                if (i != n - 1) expLength++;  // calc a ',' if exp > 1
                beyondLimit = cur_columns() + expLength > config_.get<int>("column_limit");
            }
        }
        if (beyondLimit) {
            bool hasIncIndentForAlign = false;
            if (hasIncIndent) {
                cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                cur_writer() << indentWithAlign();
            } else {
                if (config_.get<bool>("align_args")) {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                    incIndentForAlign(firstArgsIndent);
                    cur_writer() << indentWithAlign();
                    hasIncIndentForAlign = true;
                } else {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                    cur_writer() << indentWithAlign();
                }
            }
            visitExp(ctx->exp()[i + 1]);
            if (hasIncIndentForAlign) {
                decIndentForAlign(firstArgsIndent);
            }
        } else {
            bool hasIncIndentForAlign = false;
            if (config_.get<bool>("align_args")) {
                incIndentForAlign(firstArgsIndent);
                hasIncIndentForAlign = true;
            }
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
            visitExp(ctx->exp()[i + 1]);
            if (hasIncIndentForAlign) {
                decIndentForAlign(firstArgsIndent);
            }
        }
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->exp()[i + 1], "");
        }
    }
    if (hasIncIndent) {
        decContinuationIndent();
    }
    LOG_FUNCTION_END();
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
    LOG_FUNCTION_BEGIN();
    if (ctx->linkOperator() != NULL) {
        visitExp(ctx->exp()[0]);
        bool hasIncIndent = false;
        if (config_.get<bool>("break_after_operator")) {
            cur_writer() << commentAfter(ctx->exp()[0], " ");
            cur_writer() << ctx->linkOperator()->getText();
            bool beyondLimit = false;
            pushWriter();
            visitExp(ctx->exp()[1]);
            int length = cur_writer().firstLineColumn();
            length++;  // calc the white space after operator
            popWriter();
            beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
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
            beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
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
    } else if (ctx->string() != NULL) {
        visitString(ctx->string());
    } else {
        cur_writer() << ctx->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitString(LuaParser::StringContext* ctx) {
    if (ctx->NORMALSTRING() || ctx->CHARSTRING()) {
        char quote = ctx->NORMALSTRING() ? '\'' : '\"';
        tree::TerminalNode* tn;

        switch (quote) {
            case '\"':
                if (!config_.get<bool>("single_quote_to_double_quote")) goto out;
                tn = ctx->CHARSTRING();
                break;
            case '\'':
                if (!config_.get<bool>("double_quote_to_single_quote")) goto out;
                tn = ctx->NORMALSTRING();
                break;
            default:
                goto out;
        }

        string newstr = tn->getSymbol()->getText();

        regex re_single("'", regex_constants::extended);
        regex re_double("\"", regex_constants::extended);
        regex re_escapedsingle("\\\\'", regex_constants::extended);
        regex re_escapeddouble("\\\\\"", regex_constants::extended);

        if (quote == '\"') {
            newstr = regex_replace(newstr, re_escapedsingle, "'");
            newstr = regex_replace(newstr, re_double, "\\\"");
        } else {
            newstr = regex_replace(newstr, re_single, "\\'");
            newstr = regex_replace(newstr, re_escapeddouble, "\"");
        }

        // switch the beginning and end to the new format
        *newstr.begin() = quote;
        *newstr.rbegin() = quote;

        // undo a transformation that invalidates strings in certain conditions
        if (newstr.at(newstr.size() - 2) == '\\' && newstr.at(newstr.size() - 3) != '\\')
            newstr.insert(newstr.size() - 2, "\\");

        cur_writer() << newstr;
        return nullptr;
    }

out:
    cur_writer() << ctx->getText();
    return nullptr;
}

// varOrExp nameAndArgs*;
antlrcpp::Any FormatVisitor::visitPrefixexp(LuaParser::PrefixexpContext* ctx) {
    LOG_FUNCTION_BEGIN();
    chainedMethodCallHasIncIndent_.push_back(false);
    chainedMethodCallIsFirst_.push_back(false);
    visitVarOrExp(ctx->varOrExp());

    string ws = buildFirstArgumentWs(ctx->nameAndArgs());
    if (ctx->nameAndArgs().size() > 0) {
        cur_writer() << commentAfter(ctx->varOrExp(), ws);
    }

    buildArguments(ctx->nameAndArgs());

    if (chainedMethodCallHasIncIndent_.back()) {
        decContinuationIndent();
    }
    chainedMethodCallHasIncIndent_.pop_back();
    chainedMethodCallIsFirst_.pop_back();
    LOG_FUNCTION_END();
    return nullptr;
}

// var | LP exp RP;
antlrcpp::Any FormatVisitor::visitVarOrExp(LuaParser::VarOrExpContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (ctx->exp() != NULL) {
        cur_writer() << ctx->LP()->getText();
        cur_writer() << commentAfter(ctx->LP(), "");
        visitExp(ctx->exp());
        cur_writer() << commentAfter(ctx->exp(), "");
        cur_writer() << ctx->RP()->getText();

    } else {
        visitVar(ctx->var());
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// (NAME | LP exp RP varSuffix) varSuffix*;
antlrcpp::Any FormatVisitor::visitVar(LuaParser::VarContext* ctx) {
    LOG_FUNCTION_BEGIN();
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
    LOG_FUNCTION_END();
    return nullptr;
}

// nameAndArgs* (LSB exp RSB | DOT NAME);
antlrcpp::Any FormatVisitor::visitVarSuffix(LuaParser::VarSuffixContext* ctx) {
    LOG_FUNCTION_BEGIN();
    chainedMethodCallIsFirst_.push_back(false);
    chainedMethodCallHasIncIndent_.push_back(false);
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
            bool beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
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
    if (chainedMethodCallHasIncIndent_.back()) {
        decContinuationIndent();
    }
    chainedMethodCallHasIncIndent_.pop_back();
    chainedMethodCallIsFirst_.pop_back();
    LOG_FUNCTION_END();
    return nullptr;
}

// visit next NameAndArgsContext of VarSuffixContext
void FormatVisitor::visitNextNameAndArgs(LuaParser::VarSuffixContext* ctx) {
    // find args of NAME
    LuaParser::VarContext* varCtx = dynamic_cast<LuaParser::VarContext*>(ctx->parent);
    const vector<LuaParser::VarSuffixContext*>& arr = varCtx->varSuffix();
    unsigned int index = find(arr.begin(), arr.end(), ctx) - arr.begin();
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
    LOG_FUNCTION_BEGIN();
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
            beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
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
    LOG_FUNCTION_END();
    return nullptr;
}

// LP explist? RP | tableconstructor | string;
antlrcpp::Any FormatVisitor::visitArgs(LuaParser::ArgsContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (ctx->LP() != NULL) {
        cur_writer() << ctx->LP()->getText();
        if (ctx->explist() != NULL) {
            bool beyondLimit = false;
            bool breakAfterLp = false;
            pushWriterWithColumn();
            // if (!config_.get<bool>("break_before_functioncall_rp()) cur_writer") << "-";
            cur_writer() << commentAfter(ctx->LP(), "");
            visitExplist(ctx->explist());
            cur_writer() << commentAfter(ctx->explist(), "");
            int length = cur_writer().firstLineColumn();
            int lines = cur_writer().lines();
            if (!config_.get<bool>("break_before_functioncall_rp")) length++;
            popWriter();
            beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
            if (beyondLimit || lines > 1) breakAfterLp = config_.get<bool>("break_after_functioncall_lp");
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
                if (config_.get<bool>("break_before_functioncall_rp")) {
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
    LOG_FUNCTION_END();
    return nullptr;
}

// FUNCTION funcbody;
antlrcpp::Any FormatVisitor::visitFunctiondef(LuaParser::FunctiondefContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), "");
    // disable indentForAlign_ in function body
    int temp = indentForAlign_;
    indentForAlign_ = 0;
    visitFuncbody(ctx->funcbody());
    indentForAlign_ = temp;
    LOG_FUNCTION_END();
    return nullptr;
}

// LP parlist? RP block END
antlrcpp::Any FormatVisitor::visitFuncbody(LuaParser::FuncbodyContext* ctx) {
    LOG_FUNCTION_BEGIN();
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
        int lines = cur_writer().lines();
        popWriter();
        beyondLimit = lines > 1 || cur_columns() + length > config_.get<int>("column_limit");
        if (beyondLimit) {
            breakAfterLp = config_.get<bool>("break_after_functiondef_lp");
        }
        if (breakAfterLp) {
            cur_writer() << commentAfterNewLine(ctx->LP(), INC_CONTINUATION_INDENT);
            cur_writer() << indent();
        } else {
            cur_writer() << commentAfter(ctx->LP(), "");
        }

        visitParlist(ctx->parlist());

        if (config_.get<bool>("break_before_functiondef_rp")) {
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
    visitBlockAndComment(ctx->RP(), ctx->block(), FUNCTION_BLOCK);
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// namelist (COMMA ELLIPSIS)? | ELLIPSIS
antlrcpp::Any FormatVisitor::visitParlist(LuaParser::ParlistContext* ctx) {
    LOG_FUNCTION_BEGIN();
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
    LOG_FUNCTION_END();
    return nullptr;
}

// LB fieldlist? RB;
antlrcpp::Any FormatVisitor::visitTableconstructor(LuaParser::TableconstructorContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->LB()->getText();
    // disable indentForAlign_ in table
    int temp = indentForAlign_;
    indentForAlign_ = 0;
    if (ctx->fieldlist() != NULL) {
        bool containsKv = false;
        bool chopDown = false;
        if (config_.get<bool>("chop_down_kv_table")) {
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
            beyondLimit = cur_columns() + length > config_.get<int>("column_limit") || lines > 1;
        }
        bool breakAfterLb = false;
        if (beyondLimit) {
            breakAfterLb = config_.get<bool>("break_after_table_lb");
            chopDown = config_.get<bool>("chop_down_table") || (config_.get<bool>("chop_down_kv_table") && containsKv);
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
        bool needExtraFieldSep = config_.get<bool>("extra_sep_at_table_end") && ctx->fieldlist()->field().size() > 0;
        if (chopDown) {
            if (needExtraFieldSep) cur_writer() << config_.get<string>("table_sep");
            cur_writer() << commentAfterNewLine(ctx->fieldlist(), DEC_INDENT);
            cur_writer() << indent();
        } else {
            if (breakAfterLb) {
                if (config_.get<bool>("break_before_table_rb")) {
                    if (needExtraFieldSep) cur_writer() << config_.get<string>("table_sep");
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
    LOG_FUNCTION_END();
    return nullptr;
}

// field (fieldsep field)* fieldsep?;
antlrcpp::Any FormatVisitor::visitFieldlist(LuaParser::FieldlistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    auto n = ctx->field().size();
    auto sn = ctx->fieldsep().size();
    if (config_.get<bool>("align_table_field")) {
        firstTableFieldColumn_.push_back(cur_columns());
    }
    bool hasIncIndent = cur_writer().isLastCharWhiteSpace();
    bool customIncIndent = false;
    visitField(ctx->field().front());
    if (n > 1 || sn > 0) {
        cur_writer() << commentAfter(ctx->field().front(), "");
    }
    for (unsigned int i = 1; i < n; i++) {
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
        if (i != n - 1 || config_.get<bool>("extra_sep_at_table_end")) length++;  // calc a ',' if exp >1
        beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
        if (beyondLimit) {
            if (config_.get<bool>("align_table_field")) {
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
    if (config_.get<bool>("align_table_field")) {
        firstTableFieldColumn_.pop_back();
    }
    if (customIncIndent) {
        decIndent();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// LSB exp RSB EQL exp | NAME EQL exp | exp;
antlrcpp::Any FormatVisitor::visitField(LuaParser::FieldContext* ctx) {
    LOG_FUNCTION_BEGIN();
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
    LOG_FUNCTION_END();
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitFieldsep(LuaParser::FieldsepContext* context) {
    cur_writer() << config_.get<string>("table_sep");
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

bool FormatVisitor::needKeepBlockOneLine(tree::ParseTree* previousNode, LuaParser::BlockContext* ctx,
                                         BlockType blockType) {
    if (blockType == CONTROL_BLOCK && !config_.get<bool>("keep_simple_control_block_one_line")) {
        return false;
    } else if (blockType == FUNCTION_BLOCK && !config_.get<bool>("keep_simple_function_one_line")) {
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
    if (cur_columns() + length > config_.get<int>("column_limit") || lines > 1) {
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

void FormatVisitor::visitBlockAndComment(tree::ParseTree* previousNode, LuaParser::BlockContext* ctx,
                                         BlockType blockType) {
    LOG_FUNCTION_BEGIN();
    bool oneline = needKeepBlockOneLine(previousNode, ctx, blockType);
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
    LOG_FUNCTION_END();
}

string FormatVisitor::indent() {
    stringstream ss;
    for (int i = 0; i < indent_; i++) {
        if (config_.get<bool>("use_tab")) {
            ss << "\t";
        } else {
            ss << " ";
        }
    }
    return ss.str();
}

string FormatVisitor::indentWithAlign() {
    stringstream ss;
    if (config_.get<bool>("use_tab")) {
        for (int i = 0; i < indentForAlign_; i++) {
            ss << " ";
        }
        for (int i = 0; i < indent_; i++) {
            ss << "\t";
        }
    } else {
        for (int i = 0; i < indent_ + indentForAlign_; i++) {
            ss << " ";
        }
    }
    return ss.str();
}

void FormatVisitor::incIndent() {
    LOG_FUNCTION_BEGIN();
    indent_ += config_.get<int>("indent_width");
    LOG_FUNCTION_END();
}

void FormatVisitor::decIndent() {
    LOG_FUNCTION_BEGIN();
    indent_ -= config_.get<int>("indent_width");
    LOG_FUNCTION_END();
}

void FormatVisitor::incContinuationIndent() {
    LOG_FUNCTION_BEGIN();
    indent_ += config_.get<int>("continuation_indent_width");
    LOG_FUNCTION_END();
}

void FormatVisitor::decContinuationIndent() {
    LOG_FUNCTION_BEGIN();
    indent_ -= config_.get<int>("continuation_indent_width");
    LOG_FUNCTION_END();
}

void FormatVisitor::incIndentForAlign(int indent) {
    LOG_FUNCTION_BEGIN();
    indentForAlign_ += indent;
    LOG_FUNCTION_END();
}

void FormatVisitor::decIndentForAlign(int indent) {
    LOG_FUNCTION_BEGIN();
    indentForAlign_ -= indent;
    LOG_FUNCTION_END();
}
