#include <utility>

#include "LuaLexer.h"
#include "visitor/FormatVisitor.h"

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

// nameattrib (COMMA nameattrib)*;
antlrcpp::Any FormatVisitor::visitAttnamelist(LuaParser::AttnamelistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    int n = ctx->COMMA().size();
    bool hasIncIndent = false;
    unsigned firstParameterLength = ctx->nameattrib().front()->getText().size();
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
    visitNameattrib(ctx->nameattrib().front());
    if (n > 0) {
        cur_writer() << commentAfter(ctx->nameattrib().front(), "");
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        bool beyondLimit = false;
        pushWriter();
        cur_writer() << commentAfter(ctx->COMMA()[i], " ");
        visitNameattrib(ctx->nameattrib()[i + 1]);
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
            visitNameattrib(ctx->nameattrib()[i + 1]);
            if (hasIncIndentForAlign) {
                decIndentForAlign(firstParameterIndent);
            }
        } else {
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
            visitNameattrib(ctx->nameattrib()[i + 1]);
        }
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->nameattrib()[i + 1], "");
        }
    }
    if (hasIncIndent) {
        decContinuationIndent();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// NAME (LT NAME GT)?;
antlrcpp::Any FormatVisitor::visitNameattrib(LuaParser::NameattribContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->NAME().front()->getText();
    if (ctx->LT() != nullptr) {
        cur_writer() << commentAfter(ctx->NAME().front(), "");
        cur_writer() << ctx->LT()->getText();
        cur_writer() << ctx->NAME()[1]->getText();
        cur_writer() << ctx->GT()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}