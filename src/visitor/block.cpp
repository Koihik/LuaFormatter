#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

// stat* retstat?
antlrcpp::Any FormatVisitor::visitBlock(LuaParser::BlockContext* ctx) {
    LOG_FUNCTION_BEGIN();
    auto stats = ctx->stat();
    int n = stats.size();
    for (int i = 0; i < n; i++) {
        bool isSemi = stats[i]->SEMI() != nullptr;
        bool needComment = i < n - 1 || ctx->retstat() != nullptr;
        bool isNextSemi = i + 1 < n && stats[i + 1]->SEMI() != nullptr;
        if (!isSemi) {
            visitStat(stats[i]);
        }
        bool previousWhiteSpace = isSemi || cur_writer().isLastCharWhiteSpace();
        if (needComment) {
            if (isNextSemi) {
                std::string comment = commentAfter(stats[i], "");
                if (previousWhiteSpace && !comment.empty() && comment[0] == ' ') {
                    cur_writer() << comment.substr(1, comment.size() - 1);
                } else {
                    cur_writer() << comment;
                }
            } else {
                std::string comment = commentAfterNewLine(stats[i], NONE_INDENT);
                if (previousWhiteSpace && !comment.empty() && comment[0] == ' ') {
                    cur_writer() << comment.substr(1, comment.size() - 1);
                } else {
                    cur_writer() << comment;
                }
            }
        }
    }
    if (ctx->retstat() != nullptr) {
        visitRetstat(ctx->retstat());
    }
    if (ctx->retstat() == nullptr && cur_writer().isLastCharWhiteSpace()) {
        std::string blockComment = commentAfter(ctx, "");
        if (!blockComment.empty() && blockComment[0] == ' ') {
            cur_writer().ss().seekp(-1, std::ios::end);
        }
    }
    LOG_FUNCTION_END();
    return nullptr;
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