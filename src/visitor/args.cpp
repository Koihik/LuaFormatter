#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;


// LP explist? RP | tableconstructor | string;
antlrcpp::Any FormatVisitor::visitArgs(LuaParser::ArgsContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (ctx->LP() != nullptr) {
        cur_writer() << ctx->LP()->getText();
        if (ctx->explist() != nullptr) {
            bool breakAfterLp = false;
            auto p = calcASTLengthAndLines(ctx->explist(), [=]() {
                cur_writer() << commentAfter(ctx->LP(), "");
                visitExplist(ctx->explist());
                cur_writer() << commentAfter(ctx->explist(), "");
            });
            int length = p.first;
            int lines = p.second;
            if (!config_.get<bool>("break_before_functioncall_rp")) {
                length++;
            }
            bool beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
            if (beyondLimit || lines > 1) {
                breakAfterLp = config_.get<bool>("break_after_functioncall_lp");
            }
            if (config_.get<bool>("spaces_inside_functioncall_parens") && !breakAfterLp) {
                cur_writer() << " ";
            }
            functioncallLpHasBreak_.push_back(breakAfterLp);
            if (breakAfterLp) {
                // break line on '(' keeping comments
                cur_writer() << commentAfterNewLine(ctx->LP(), INC_CONTINUATION_INDENT);
                cur_writer() << indentWithAlign();
            } else {
                cur_writer() << commentAfter(ctx->LP(), "");
            }
            visitExplist(ctx->explist());
            // 'visitExplist' will leave 'indent_' on 0 if we detect that it will break line
            // on '(' by itself, therefore we cannot decrement 'indent_'
            if (config_.get<bool>("break_before_functioncall_rp")) {
                if (breakAfterLp) {
                    cur_writer() << commentAfterNewLine(ctx->explist(), DEC_CONTINUATION_INDENT);
                    cur_writer() << indentWithAlign();
                } else {
                    cur_writer() << commentAfter(ctx->explist(), "");
                }
            } else {
                if (breakAfterLp) {
                    decContinuationIndent();
                }
                cur_writer() << commentAfter(ctx->explist(), "");
            }
            if (config_.get<bool>("spaces_inside_functioncall_parens")) {
                cur_writer() << " ";
            }
            functioncallLpHasBreak_.pop_back();
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
