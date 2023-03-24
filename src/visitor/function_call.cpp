#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

// varOrExp nameAndArgs+;
antlrcpp::Any FormatVisitor::visitFunctioncall(LuaParser::FunctioncallContext* ctx) {
    LOG_FUNCTION_BEGIN();
    chainedMethodCallHasIncIndent_.push_back(false);
    chainedMethodCallIsFirst_.push_back(false);
    visitVarOrExp(ctx->varOrExp());

    std::string ws = buildFirstArgumentWs(ctx->nameAndArgs());
    cur_writer() << commentAfter(ctx->varOrExp(), ws);

    buildArguments(ctx->nameAndArgs());

    if (ctx->SEMI() != nullptr) {
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
