#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

// Builds first argument whitespace in spaces.
// local a = f<WS>{1, 2, 3} or f<WS>"a"
std::string FormatVisitor::buildFirstArgumentWs(std::vector<LuaParser::NameAndArgsContext*> v) {
    bool needWhiteSpace = false;

    if (!v.empty()) {
        LuaParser::NameAndArgsContext* ctx = v.front();
        if (ctx->COLON() == nullptr && ctx->args()->LP() == nullptr) {
            needWhiteSpace = true;
        }
    }

    // Build whitespace
    std::string ws;
    if (needWhiteSpace) {
        int sz = config_.get<int>("spaces_before_call");
        while ((sz--) != 0) {
            ws.append(" ");
        }
    }

    return ws;
}

// Builds arguments whitespaces in spaces.
// local a = f"a"<WS>"b"<WS>"c"...
void FormatVisitor::buildArguments(std::vector<LuaParser::NameAndArgsContext*> v) {
    for (size_t i = 0; i < v.size(); i++) {
        auto* ctx = v[i];
        std::string ws;

        // Write out the argument
        visitNameAndArgs(ctx);

        if (ctx == v.back()) {  // Last element, bail out
            break;
        }

        auto* nctx = v[i + 1];
        if (nctx->COLON() == nullptr &&
            ((nctx->args()->string() != nullptr) || (nctx->args()->tableconstructor() != nullptr))) {
            ws = " ";
        }

        cur_writer() << commentAfter(ctx, ws);
    }
}
