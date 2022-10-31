#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

// varlist EQL explist;
antlrcpp::Any FormatVisitor::visitVarDecl(LuaParser::VarDeclContext* ctx) {
    LOG_FUNCTION_BEGIN();
    visitVarlist(ctx->varlist());
    cur_writer() << commentAfter(ctx->varlist(), " ");
    cur_writer() << ctx->EQL()->getText();
    cur_writer() << commentAfter(ctx->EQL(), " ");
    visitExplist(ctx->explist());
    if (ctx->SEMI() != nullptr) {
        cur_writer() << commentAfter(ctx->explist(), "");
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

// LOCAL attnamelist (EQL explist)? SEMI?;
antlrcpp::Any FormatVisitor::visitLocalVarDecl(LuaParser::LocalVarDeclContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->LOCAL()->getText();
    cur_writer() << commentAfter(ctx->LOCAL(), " ");
    visitAttnamelist(ctx->attnamelist());
    if (ctx->EQL() != nullptr) {
        cur_writer() << commentAfter(ctx->attnamelist(), " ");
        cur_writer() << ctx->EQL()->getText();
        cur_writer() << commentAfter(ctx->EQL(), " ");
        visitExplist(ctx->explist());
    }
    if (ctx->SEMI() != nullptr) {
        if (ctx->EQL() == nullptr) {
            cur_writer() << commentAfter(ctx->attnamelist(), "");
        } else {
            cur_writer() << commentAfter(ctx->explist(), "");
        }
        cur_writer() << ctx->SEMI()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// NAME attrib (COMMA NAME attrib)*;
antlrcpp::Any FormatVisitor::visitAttnamelist(LuaParser::AttnamelistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    int n = ctx->COMMA().size();
    bool hasIncIndent = false;
    unsigned firstParameterLength = ctx->NAME().front()->getText().size() + ctx->attrib().front()->getText().size();
    int firstParameterIndent = 0;

    if (n > 0) {
        firstParameterLength++;  // calc a ',' if exp > 1
    }
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
    std::string attrib = ctx->attrib().front()->getText();
    // judge attrib size, avoid print comment twice
    if (!attrib.empty()) {
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
        if (i != n - 1) {
            length++;  // calc a ',' if exp > 1
        }
        beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
        if (beyondLimit) {
            bool hasIncIndentForAlign = false;
            if (hasIncIndent) {
                cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                cur_writer() << indentWithAlign();
            } else {
                if (config_.get<bool>("align_parameter")) {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                    incIndentForAlign(firstParameterIndent);
                    cur_writer() << indentWithAlign();
                    hasIncIndentForAlign = true;
                } else {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], INC_CONTINUATION_INDENT);
                    cur_writer() << indentWithAlign();
                    hasIncIndent = true;
                }
            }
            cur_writer() << ctx->NAME()[i + 1]->getText();
            cur_writer() << ctx->attrib()[i + 1]->getText();
            if (hasIncIndentForAlign) {
                decIndentForAlign(firstParameterIndent);
            }
        } else {
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
            cur_writer() << ctx->NAME()[i + 1]->getText();
            cur_writer() << ctx->attrib()[i + 1]->getText();
        }
        if (i != n - 1) {
            if (!ctx->attrib()[i + 1]->getText().empty()) {
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
    if (ctx->NAME() != nullptr) {
        cur_writer() << ctx->LT()->getText();
        cur_writer() << ctx->NAME()->getText();
        cur_writer() << ctx->GT()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}