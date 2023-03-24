#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;


// FUNCTION funcname funcbody;
antlrcpp::Any FormatVisitor::visitFuncStat(LuaParser::FuncStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), " ");
    visitFuncname(ctx->funcname());
    cur_writer() << commentAfter(ctx->funcname(), "");
    visitFuncbody(ctx->funcbody());
    LOG_FUNCTION_END();
    return nullptr;
}

// LOCAL FUNCTION NAME funcbody;
antlrcpp::Any FormatVisitor::visitLocalFuncStat(LuaParser::LocalFuncStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->LOCAL()->getText();
    cur_writer() << commentAfter(ctx->LOCAL(), " ");
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), " ");
    cur_writer() << ctx->NAME()->getText();
    cur_writer() << commentAfter(ctx->NAME(), "");
    visitFuncbody(ctx->funcbody());
    LOG_FUNCTION_END();
    return nullptr;
}

// FUNCTION funcbody;
antlrcpp::Any FormatVisitor::visitFunctiondef(LuaParser::FunctiondefContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->FUNCTION()->getText();
    cur_writer() << commentAfter(ctx->FUNCTION(), "");
    // disable indentForAlign_ in function body
    int temp = indentForAlign_;
    indentForAlign_ = 0;
    visitFuncbody(ctx->funcbody());
    indentForAlign_ = temp;
    LOG_FUNCTION_END();
    return nullptr;
}

// LP parlist? RP block END
antlrcpp::Any FormatVisitor::visitFuncbody(LuaParser::FuncbodyContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->LP()->getText();
    if (ctx->parlist() != nullptr) {
        bool breakAfterLp = false;
        bool beyondLimit = false;
        pushWriter();
        cur_writer() << commentAfter(ctx->LP(), "");
        visitParlist(ctx->parlist());
        cur_writer() << commentAfter(ctx->parlist(), "");
        cur_writer() << ctx->RP()->getText();
        int length = cur_writer().firstLineColumn();
        int lines = cur_writer().lines();
        popWriter();
        beyondLimit = lines > 1 || cur_columns() + length > config_.get<int>("column_limit");
        if (beyondLimit) {
            breakAfterLp = config_.get<bool>("break_after_functiondef_lp");
        }
        if (config_.get<bool>("spaces_inside_functiondef_parens") && !beyondLimit && !breakAfterLp) {
            cur_writer() << " ";
        }
        if (breakAfterLp) {
            cur_writer() << commentAfterNewLine(ctx->LP(), INC_CONTINUATION_INDENT);
            cur_writer() << indent();
        } else {
            cur_writer() << commentAfter(ctx->LP(), "");
        }
        breakAfterLpHasIncIndent_.push_back(breakAfterLp);
        visitParlist(ctx->parlist());
        breakAfterLpHasIncIndent_.pop_back();

        if (config_.get<bool>("break_before_functiondef_rp")) {
            if (breakAfterLp) {
                cur_writer() << commentAfterNewLine(ctx->parlist(), DEC_CONTINUATION_INDENT);
                cur_writer() << indent();
            } else {
                cur_writer() << commentAfter(ctx->parlist(), "");
            }
        } else {
            if (breakAfterLp) {
                decContinuationIndent();
            }
            cur_writer() << commentAfter(ctx->parlist(), "");
        }
    } else {
        cur_writer() << commentAfter(ctx->LP(), "");
    }
    if (config_.get<bool>("spaces_inside_functiondef_parens") && ctx->parlist() != nullptr) {
        cur_writer() << " ";
    }
    cur_writer() << ctx->RP()->getText();
    visitBlockAndComment(ctx->RP(), ctx->block(), FUNCTION_BLOCK);
    cur_writer() << ctx->END()->getText();
    auto lineBrakesCount = config_.get<int>("line_breaks_after_function_body");
    if (lineBrakesCount > 1) {
        for (size_t i = 1; i < lineBrakesCount; i++) {
            cur_writer() << "\n";
        }
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// namelist (COMMA ELLIPSIS)? | ELLIPSIS
antlrcpp::Any FormatVisitor::visitParlist(LuaParser::ParlistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (ctx->namelist() != nullptr) {
        visitNamelist(ctx->namelist());
        if (ctx->COMMA() != nullptr) {
            cur_writer() << commentAfter(ctx->namelist(), "");
            cur_writer() << ctx->COMMA()->getText();
            cur_writer() << commentAfter(ctx->COMMA(), " ");
            cur_writer() << ctx->ELLIPSIS()->getText();
        }
    } else {
        cur_writer() << ctx->ELLIPSIS()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}
