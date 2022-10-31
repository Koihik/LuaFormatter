#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

// NAME (COMMA NAME)*;
antlrcpp::Any FormatVisitor::visitNamelist(LuaParser::NamelistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    int n = ctx->COMMA().size();
    bool hasIncIndent = false;
    int firstParameterLength = ctx->NAME().front()->getText().size();
    int firstParameterIndent = 0;
    bool incIndentByFuncdef = !breakAfterLpHasIncIndent_.empty() && breakAfterLpHasIncIndent_.back();

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
    if (incIndentByFuncdef) {
        hasIncIndent = true;
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
            if (i != n - 1) {
                length++;  // calc a ',' if exp > 1
            }
            beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
        }
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
            if (hasIncIndentForAlign) {
                decIndentForAlign(firstParameterIndent);
            }
        } else {
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
            cur_writer() << ctx->NAME()[i + 1]->getText();
        }
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->NAME()[i + 1], "");
        }
    }
    if (hasIncIndent && !incIndentByFuncdef) {
        decContinuationIndent();
    }
    LOG_FUNCTION_END();
    return nullptr;
}
