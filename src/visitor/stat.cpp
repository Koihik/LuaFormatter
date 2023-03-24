#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

antlrcpp::Any FormatVisitor::visitStat(LuaParser::StatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (chop_down_block_) {
        cur_writer() << indent();
    }
    visitChildren(ctx);
    LOG_FUNCTION_END();
    return nullptr;
}

// GOTO NAME;
antlrcpp::Any FormatVisitor::visitGotoStat(LuaParser::GotoStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->GOTO()->getText();
    cur_writer() << commentAfter(ctx->GOTO(), " ");
    cur_writer() << ctx->NAME()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// DO block END;
antlrcpp::Any FormatVisitor::visitDoStat(LuaParser::DoStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// WHILE exp DO block END;
antlrcpp::Any FormatVisitor::visitWhileStat(LuaParser::WhileStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->WHILE()->getText();
    cur_writer() << commentAfter(ctx->WHILE(), " ");
    visitExp(ctx->exp());
    cur_writer() << commentAfter(ctx->exp(), " ");
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// REPEAT block UNTIL exp;
antlrcpp::Any FormatVisitor::visitRepeatStat(LuaParser::RepeatStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->REPEAT()->getText();
    visitBlockAndComment(ctx->REPEAT(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->UNTIL()->getText();
    cur_writer() << commentAfter(ctx->UNTIL(), " ");
    visitExp(ctx->exp());
    if (ctx->SEMI() != nullptr) {
        cur_writer() << commentAfter(ctx->exp(), "");
        cur_writer() << ctx->SEMI()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// IF exp THEN block (ELSEIF exp THEN block)* (ELSE block)? END;
antlrcpp::Any FormatVisitor::visitIfStat(LuaParser::IfStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    // keep if statement one line only when there is no elseif or else statement.
    cur_writer() << ctx->IF()->getText();
    cur_writer() << commentAfter(ctx->IF(), " ");
    visitExp(ctx->exp().front());
    cur_writer() << commentAfter(ctx->exp().front(), " ");
    cur_writer() << ctx->THEN().front()->getText();
    if (ctx->ELSEIF().empty() && ctx->ELSE() == nullptr) {
        if (needKeepBlockOneLine(ctx->THEN().front(), ctx->block().front(), CONTROL_BLOCK)) {
            cur_writer() << commentAfter(ctx->THEN().front(), " ");
            bool temp = chop_down_block_;
            chop_down_block_ = false;
            visitBlock(ctx->block().front());
            chop_down_block_ = temp;
            if (!isBlockEmpty(ctx->block().front())) {
                cur_writer() << commentAfter(ctx->block().front(), " ");
            }
            cur_writer() << ctx->END()->getText();
            return nullptr;
        }
    }
    cur_writer() << commentAfterNewLine(ctx->THEN().front(), INC_INDENT);
    visitBlock(ctx->block().front());
    cur_writer() << commentAfterNewLine(ctx->block().front(), DEC_INDENT);
    int n = ctx->ELSEIF().size();
    for (int i = 0; i < n; i++) {
        cur_writer() << indent();
        cur_writer() << ctx->ELSEIF()[i]->getText();
        cur_writer() << commentAfter(ctx->ELSEIF()[i], " ");
        visitExp(ctx->exp()[i + 1]);
        cur_writer() << commentAfter(ctx->exp()[i + 1], " ");
        cur_writer() << ctx->THEN()[i + 1]->getText();
        cur_writer() << commentAfterNewLine(ctx->THEN()[i + 1], INC_INDENT);
        visitBlock(ctx->block()[i + 1]);
        cur_writer() << commentAfterNewLine(ctx->block()[i + 1], DEC_INDENT);
    }
    if (ctx->ELSE() != nullptr) {
        cur_writer() << indent();
        cur_writer() << ctx->ELSE()->getText();
        cur_writer() << commentAfterNewLine(ctx->ELSE(), INC_INDENT);
        visitBlock(ctx->block().back());
        cur_writer() << commentAfterNewLine(ctx->block().back(), DEC_INDENT);
    }
    cur_writer() << indent();  //
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// FOR NAME EQL exp COMMA exp (COMMA exp)? DO block END;
antlrcpp::Any FormatVisitor::visitForStat(LuaParser::ForStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->FOR()->getText();
    cur_writer() << commentAfter(ctx->FOR(), " ");
    cur_writer() << ctx->NAME()->getText();
    cur_writer() << commentAfter(ctx->NAME(), " ");
    cur_writer() << ctx->EQL()->getText();
    cur_writer() << commentAfter(ctx->EQL(), " ");
    visitExp(ctx->exp()[0]);
    cur_writer() << commentAfter(ctx->exp()[0], "");
    cur_writer() << ctx->COMMA()[0]->getText();
    cur_writer() << commentAfter(ctx->COMMA()[0], " ");
    visitExp(ctx->exp()[1]);
    if (ctx->COMMA().size() > 1) {
        cur_writer() << commentAfter(ctx->exp()[1], "");
        cur_writer() << ctx->COMMA()[1]->getText();
        cur_writer() << commentAfter(ctx->COMMA()[1], " ");
        visitExp(ctx->exp()[2]);
        cur_writer() << commentAfter(ctx->exp()[2], " ");
    } else {
        cur_writer() << commentAfter(ctx->exp()[1], " ");
    }
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// FOR namelist IN explist DO block END;
antlrcpp::Any FormatVisitor::visitForInStat(LuaParser::ForInStatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->FOR()->getText();
    cur_writer() << commentAfter(ctx->FOR(), " ");
    visitNamelist(ctx->namelist());
    cur_writer() << commentAfter(ctx->namelist(), " ");
    cur_writer() << ctx->IN()->getText();
    cur_writer() << commentAfter(ctx->IN(), " ");
    visitExplist(ctx->explist());
    cur_writer() << commentAfter(ctx->explist(), " ");
    cur_writer() << ctx->DO()->getText();
    visitBlockAndComment(ctx->DO(), ctx->block(), CONTROL_BLOCK);
    cur_writer() << ctx->END()->getText();
    LOG_FUNCTION_END();
    return nullptr;
}

// RETURN explist? SEMI?;
antlrcpp::Any FormatVisitor::visitRetstat(LuaParser::RetstatContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (chop_down_block_) {
        cur_writer() << indent();
    }
    cur_writer() << ctx->RETURN()->getText();
    if (ctx->explist() != nullptr) {
        cur_writer() << commentAfter(ctx->RETURN(), " ");
        visitExplist(ctx->explist());
    }
    if (ctx->SEMI() != nullptr) {
        cur_writer() << ctx->SEMI()->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}