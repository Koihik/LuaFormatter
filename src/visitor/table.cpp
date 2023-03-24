#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

// LB fieldlist? RB;
antlrcpp::Any FormatVisitor::visitTableconstructor(LuaParser::TableconstructorContext* ctx) {
    LOG_FUNCTION_BEGIN();
    cur_writer() << ctx->LB()->getText();
    // disable indentForAlign_ in table
    int temp = indentForAlign_;
    indentForAlign_ = 0;
    if (ctx->fieldlist() != nullptr) {
        bool containsKv = false;
        bool chopDown = false;
        if (config_.get<bool>("chop_down_kv_table")) {
            for (auto* f : ctx->fieldlist()->field()) {
                if (f->EQL() != nullptr) {
                    containsKv = true;
                    break;
                }
            }
        }
        bool temp = chop_down_table_;
        bool beyondLimit = fastTestColumnLimit(ctx->fieldlist());
        if (!beyondLimit) {
            pushWriter();
            cur_writer() << commentAfter(ctx->LB(), "");
            chop_down_table_ = false;
            visitFieldlist(ctx->fieldlist());
            chop_down_table_ = temp;
            cur_writer() << commentAfter(ctx->fieldlist(), "");
            cur_writer() << ctx->RB()->getText();
            int length = cur_writer().firstLineColumn();
            int lines = cur_writer().lines();
            popWriter();
            int column_table_limit = 0;
            if (containsKv) {
                column_table_limit = config_.get<int>("column_table_limit_kv");
            }
            if (!column_table_limit) {
                column_table_limit = config_.get<int>("column_table_limit");
                if (!column_table_limit) {
                    column_table_limit = config_.get<int>("column_limit");
                }
            }
            
            beyondLimit = cur_columns() + length > column_table_limit || lines > 1;
        }
        bool breakAfterLb = false;
        if (beyondLimit) {
            breakAfterLb = config_.get<bool>("break_after_table_lb");
            chopDown = config_.get<bool>("chop_down_table") || (config_.get<bool>("chop_down_kv_table") && containsKv);
        }
        if (chopDown) {
            cur_writer() << commentAfterNewLine(ctx->LB(), INC_INDENT);
            cur_writer() << indent();
            chop_down_table_ = true;
        } else {
            if (breakAfterLb) {
                cur_writer() << commentAfterNewLine(ctx->LB(), INC_INDENT);
                cur_writer() << indent();
            } else {
                if (config_.get<bool>("spaces_inside_table_braces") && !breakAfterLb) {
                    cur_writer() << " ";
                }
                cur_writer() << commentAfter(ctx->LB(), "");
            }
            chop_down_table_ = false;
        }
        visitFieldlist(ctx->fieldlist());
        bool needExtraFieldSep = config_.get<bool>("extra_sep_at_table_end") && !ctx->fieldlist()->field().empty();
        if (chopDown) {
            if (needExtraFieldSep) {
                cur_writer() << config_.get<std::string>("table_sep");
            }
            cur_writer() << commentAfterNewLine(ctx->fieldlist(), DEC_INDENT);
            cur_writer() << indent();
        } else {
            if (breakAfterLb) {
                if (config_.get<bool>("break_before_table_rb")) {
                    if (needExtraFieldSep) {
                        cur_writer() << config_.get<std::string>("table_sep");
                    }
                    cur_writer() << commentAfterNewLine(ctx->fieldlist(), DEC_INDENT);
                    cur_writer() << indent();
                } else {
                    decIndent();
                    cur_writer() << commentAfter(ctx->fieldlist(), "");
                }
            } else {
                cur_writer() << commentAfter(ctx->fieldlist(), "");
            }
        }
        if (config_.get<bool>("spaces_inside_table_braces") && !breakAfterLb) {
            cur_writer() << " ";
        }
        cur_writer() << ctx->RB()->getText();
        chop_down_table_ = temp;
    } else {
        std::string comment = commentAfter(ctx->LB(), "");
        // if comment contains line break, then keep it.
        if (comment.find('\n') != std::string::npos) {
            cur_writer() << commentAfterNewLine(ctx->LB(), INC_INDENT);
            decIndent();
            cur_writer() << indent();
            cur_writer() << ctx->RB()->getText();
        } else {
            // comment not contains line break, format the table to one line.
            cur_writer() << commentAfter(ctx->LB(), "");
            cur_writer() << ctx->RB()->getText();
        }
    }
    indentForAlign_ = temp;
    LOG_FUNCTION_END();
    return nullptr;
}

// field (fieldsep field)* fieldsep?;
antlrcpp::Any FormatVisitor::visitFieldlist(LuaParser::FieldlistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    auto n = ctx->field().size();
    auto sn = ctx->fieldsep().size();
    if (config_.get<bool>("align_table_field")) {
        firstTableFieldColumn_.push_back(cur_columns());
    }
    bool hasIncIndent = cur_writer().isLastCharWhiteSpace();
    bool customIncIndent = false;
    visitField(ctx->field().front());
    if (n > 1 || sn > 0) {
        cur_writer() << commentAfter(ctx->field().front(), "");
    }
    for (unsigned int i = 1; i < n; i++) {
        if (chop_down_table_) {
            visitFieldsep(ctx->fieldsep()[i - 1]);
            cur_writer() << commentAfterNewLine(ctx->fieldsep()[i - 1], NONE_INDENT);
            cur_writer() << indent();
            visitField(ctx->field()[i]);
            if (i != n - 1 || ctx->fieldsep().size() == n) {
                cur_writer() << commentAfter(ctx->field()[i], "");
            }
            continue;
        }
        visitFieldsep(ctx->fieldsep()[i - 1]);
        bool beyondLimit = false;
        pushWriter();
        cur_writer() << commentAfter(ctx->fieldsep()[i - 1], " ");
        visitField(ctx->field()[i]);
        int length = cur_writer().firstLineColumn();
        popWriter();
        if (i != n - 1 || config_.get<bool>("extra_sep_at_table_end")) {
            length++;  // calc a ',' if exp >1
        }
        auto column_table_limit = config_.get<int>("column_table_limit");
        if (!column_table_limit) {
            column_table_limit = config_.get<int>("column_limit");
        }
        beyondLimit = cur_columns() + length > column_table_limit;
        if (beyondLimit) {
            if (config_.get<bool>("align_table_field")) {
                cur_writer() << commentAfterNewLine(ctx->fieldsep()[i - 1], NONE_INDENT);
                for (int i = 0; i < firstTableFieldColumn_.back(); i++) {
                    cur_writer() << " ";
                }
            } else {
                if (hasIncIndent) {
                    cur_writer() << commentAfterNewLine(ctx->fieldsep()[i - 1], NONE_INDENT);
                    cur_writer() << indent();
                } else {
                    cur_writer() << commentAfterNewLine(ctx->fieldsep()[i - 1], INC_INDENT);
                    cur_writer() << indent();
                    customIncIndent = true;
                    hasIncIndent = true;
                }
            }
            visitField(ctx->field()[i]);
        } else {
            cur_writer() << commentAfter(ctx->fieldsep()[i - 1], " ");
            visitField(ctx->field()[i]);
        }
        if (i != n - 1 || ctx->fieldsep().size() == n) {
            cur_writer() << commentAfter(ctx->field()[i], "");
        }
    }
    if (config_.get<bool>("align_table_field")) {
        firstTableFieldColumn_.pop_back();
    }
    if (customIncIndent) {
        decIndent();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// LSB exp RSB EQL exp | NAME EQL exp | exp;
antlrcpp::Any FormatVisitor::visitField(LuaParser::FieldContext* ctx) {
    LOG_FUNCTION_BEGIN();
    std::string eq_space = config_.get<bool>("spaces_around_equals_in_field") ? " " : "";
    if (ctx->LSB() != nullptr) {
        cur_writer() << ctx->LSB()->getText();
        cur_writer() << commentAfter(ctx->LSB(), "");
        visitExp(ctx->exp()[0]);
        cur_writer() << commentAfter(ctx->exp()[0], "");
        cur_writer() << ctx->RSB()->getText();
        cur_writer() << commentAfter(ctx->RSB(), eq_space);
        cur_writer() << ctx->EQL()->getText();
        cur_writer() << commentAfter(ctx->EQL(), eq_space);
        visitExp(ctx->exp()[1]);
    } else if (ctx->NAME() != nullptr) {
        cur_writer() << ctx->NAME()->getText();
        cur_writer() << commentAfter(ctx->NAME(), eq_space);
        cur_writer() << ctx->EQL()->getText();
        cur_writer() << commentAfter(ctx->EQL(), eq_space);
        visitExp(ctx->exp().front());
    } else {
        visitExp(ctx->exp().front());
    }
    LOG_FUNCTION_END();
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitFieldsep(LuaParser::FieldsepContext* context) {
    cur_writer() << config_.get<std::string>("table_sep");
    return nullptr;
}