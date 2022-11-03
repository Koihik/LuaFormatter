#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

// (COLON NAME)? args;
antlrcpp::Any FormatVisitor::visitNameAndArgs(LuaParser::NameAndArgsContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (ctx->COLON() != nullptr) {
        bool beyondLimit = false;
        LuaParser::StringContext* stringContext = ctx->args()->string();
        bool needWhiteSpace = stringContext != nullptr;
        if (!chainedMethodCallIsFirst_.back()) {
            chainedMethodCallIsFirst_[chainedMethodCallIsFirst_.size() - 1] = true;
        } else {
            int length = calcASTLengthAndLines(ctx->args(), [=]() {
                             cur_writer() << ctx->COLON()->getText();
                             cur_writer() << commentAfter(ctx->COLON(), "");
                             cur_writer() << ctx->NAME()->getText();
                             cur_writer() << commentAfter(ctx->NAME(), needWhiteSpace ? " " : "");
                             visitArgs(ctx->args());
                         }).first;
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
