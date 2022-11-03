#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

antlrcpp::Any FormatVisitor::visitChunk(LuaParser::ChunkContext* ctx) {
    LOG_FUNCTION_BEGIN();
    pushWriter();
    cur_writer() << commentAfter(nullptr, "");
    bool temp = chop_down_block_;
    chop_down_block_ = true;
    visitBlock(ctx->block());
    chop_down_block_ = temp;
    std::string comment = commentAfterNewLine(ctx->block(), NONE_INDENT);

    cur_writer() << comment;
    std::string ret = cur_writer().str();
    if (*ret.rbegin() != '\n') {
        ret += '\n';
    }
    popWriter();
    LOG_FUNCTION_END();
    return ret;
}