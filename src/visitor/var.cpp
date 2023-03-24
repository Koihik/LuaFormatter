#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

// (NAME | LP exp RP varSuffix) varSuffix*;
antlrcpp::Any FormatVisitor::visitVar(LuaParser::VarContext* ctx) {
    LOG_FUNCTION_BEGIN();
    int startAt = 0;
    if (ctx->exp() != nullptr) {
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
        if (!ctx->varSuffix().empty()) {
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
    for (auto* na : ctx->nameAndArgs()) {
        visitNameAndArgs(na);
        cur_writer() << commentAfter(na, "");
    }
    if (ctx->exp() != nullptr) {
        std::string expString = ctx->exp()->getText();
        // if table key is a nested string, keep the whitespace
        // example:
        // x = {}
        // x[ [[key]] ] = "value"
        if (!expString.empty() && expString[0] == '[') {
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
        auto* varCtx = dynamic_cast<LuaParser::VarContext*>(ctx->parent);
        const std::vector<LuaParser::VarSuffixContext*>& arr = varCtx->varSuffix();
        int index = find(arr.begin(), arr.end(), ctx) - arr.begin();
        if (index != 0) {
            int length = calcASTLengthAndLines(ctx->NAME(), [=]() {
                             cur_writer() << ctx->DOT()->getText();
                             cur_writer() << commentAfter(ctx->DOT(), "");
                             cur_writer() << ctx->NAME()->getText();
                             visitNextNameAndArgs(ctx);
                         }).first;
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
    auto* varCtx = dynamic_cast<LuaParser::VarContext*>(ctx->parent);
    const std::vector<LuaParser::VarSuffixContext*>& arr = varCtx->varSuffix();
    unsigned int index = find(arr.begin(), arr.end(), ctx) - arr.begin();
    if (index + 1 < arr.size()) {
        for (auto* na : arr[index + 1]->nameAndArgs()) {
            // if next NameAndArgs's COLON is not null
            // then formatter can place a break before COLON.
            // So do not need to calc column length for current NAME, break the loop.
            // example:
            // self.xxx.yyy:foo()
            // the 'xxx' and 'yyy' is a 'Variable', and 'foo' is a 'Method'
            // It better break before 'foo' than break before 'yyy'
            if (na->COLON() != nullptr) {
                break;
            }
            visitNameAndArgs(na);
            if (na != arr[index + 1]->nameAndArgs().back()) {
                cur_writer() << commentAfter(na, "");
            }
        }
    } else {
        auto* varOrExpCtx = dynamic_cast<LuaParser::VarOrExpContext*>(varCtx->parent);
        // parent maybe VarlistContext
        if (varOrExpCtx != nullptr) {
            tree::ParseTree* parent = varOrExpCtx->parent;

            auto* peCtx = dynamic_cast<LuaParser::PrefixexpContext*>(parent);
            if (peCtx != nullptr) {
                for (auto* na : peCtx->nameAndArgs()) {
                    if (na->COLON() != nullptr) {
                        break;
                    }
                    visitNameAndArgs(na);
                    if (na != peCtx->nameAndArgs().back()) {
                        cur_writer() << commentAfter(na, "");
                    }
                }
            }

            auto* fcCtx = dynamic_cast<LuaParser::FunctioncallContext*>(parent);
            if (fcCtx != nullptr) {
                for (auto* na : fcCtx->nameAndArgs()) {
                    if (na->COLON() != nullptr) {
                        break;
                    }
                    visitNameAndArgs(na);
                    if (na != fcCtx->nameAndArgs().back()) {
                        cur_writer() << commentAfter(na, "");
                    }
                }
            }
        }
    }
}

