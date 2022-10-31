#include "visitor/FormatVisitor.h"

#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

// exp (COMMA exp)*;
antlrcpp::Any FormatVisitor::visitExplist(LuaParser::ExplistContext* ctx) {
    LOG_FUNCTION_BEGIN();
    int n = ctx->COMMA().size();
    auto p = calcASTLengthAndLines(ctx->exp().front(), [=]() { visitExp(ctx->exp().front()); });
    int expLength = p.first;
    int lines = p.second;
    if (n > 0) {
        expLength++;  // calc a ',' if exp > 1
    }
    bool beyondLimit = cur_columns() + expLength > config_.get<int>("column_limit");
    // if lines > 1 and current column is already too long, then break
    // example:
    // var:foo(xxxx .. xxxx) -- no break
    // longlonglonglongvar:foo(
    //     xxxx .. xxxx) --  break
    int column_limit = config_.get<int>("column_limit");
    if (lines > 1 && cur_columns() > column_limit / 2 && expLength >= column_limit / 4) {
        beyondLimit = true;
    }
    if (!functioncallLpHasBreak_.empty() && functioncallLpHasBreak_.back()) {
        beyondLimit = false;
    }
    bool hasIncIndent = false;
    int firstArgsIndent = 0;
    if (beyondLimit) {
        if (cur_writer().isLastCharWhiteSpace()) {
            cur_writer().ss().seekp(-1, std::ios::end);
        }
        cur_writer() << "\n";
        incContinuationIndent();
        cur_writer() << indentWithAlign();
        hasIncIndent = true;
        visitExp(ctx->exp().front());
    } else {
        firstArgsIndent = cur_columns() - indent_ - indentForAlign_;
        incIndentForAlign(firstArgsIndent);
        visitExp(ctx->exp().front());
        decIndentForAlign(firstArgsIndent);
    }
    if (n > 0) {
        cur_writer() << commentAfter(ctx->exp().front(), "");
    }
    for (int i = 0; i < n; i++) {
        cur_writer() << ctx->COMMA()[i]->getText();
        bool beyondLimit = false;
        pushWriter();
        cur_writer() << commentAfter(ctx->COMMA()[i], " ");
        visitExp(ctx->exp()[i + 1]);
        int expLength = cur_writer().firstLineColumn();
        int lines = cur_writer().lines();
        popWriter();
        if (i != n - 1) {
            expLength++;  // calc a ',' if exp > 1
        }
        beyondLimit = cur_columns() + expLength > config_.get<int>("column_limit");
        if (beyondLimit && lines == 1) {
            // lines is 1 means expression is enough short. But with current columns,
            // it beyonds column limit, need a line break here.
            // But if after a line break, the expression is still beyond column limit.
            // Then don't let it break the line at the beginning.
            // example:
            // longlonglongvar.longlonglongmethod(arg11111, function() print("11111111111111111111111111111111") end)
            //
            // This function call is beyond column limit when visit the second args.
            // so if we break after ',' , it become:
            // longlonglongvar.longlonglongmethod(arg11111,
            //                                    function() print("11111111111111111111111111111111") end)
            //
            // The second args is still too long, and need another break:
            // longlonglongvar.longlonglongmethod(arg11111,
            //                                    function()
            //     print("11111111111111111111111111111111")
            // end)
            //
            // The first break (before 'function()') doesn't seem to make sense.
            // So determine if this expression is still beyond column limit after break.
            // If true, determine the column again with current columns.
            if (indentWithAlign().size() + expLength + firstArgsIndent > config_.get<unsigned int>("column_limit")) {
                int expLength = calcASTLengthAndLines(ctx->exp()[i + 1], [=]() {
                                    cur_writer() << commentAfter(ctx->COMMA()[i], " ");
                                    visitExp(ctx->exp()[i + 1]);
                                }).first;
                if (i != n - 1) expLength++;  // calc a ',' if exp > 1
                beyondLimit = cur_columns() + expLength > config_.get<int>("column_limit");
            }
        }
        if (beyondLimit) {
            bool hasIncIndentForAlign = false;
            if (hasIncIndent) {
                cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                cur_writer() << indentWithAlign();
            } else {
                if (config_.get<bool>("align_args")) {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], NONE_INDENT);
                    incIndentForAlign(firstArgsIndent);
                    cur_writer() << indentWithAlign();
                    hasIncIndentForAlign = true;
                } else {
                    cur_writer() << commentAfterNewLine(ctx->COMMA()[i], INC_CONTINUATION_INDENT);
                    cur_writer() << indentWithAlign();
                    hasIncIndent = true;
                }
            }
            visitExp(ctx->exp()[i + 1]);
            if (hasIncIndentForAlign) {
                decIndentForAlign(firstArgsIndent);
            }
        } else {
            bool hasIncIndentForAlign = false;
            if (config_.get<bool>("align_args")) {
                incIndentForAlign(firstArgsIndent);
                hasIncIndentForAlign = true;
            }
            cur_writer() << commentAfter(ctx->COMMA()[i], " ");
            visitExp(ctx->exp()[i + 1]);
            if (hasIncIndentForAlign) {
                decIndentForAlign(firstArgsIndent);
            }
        }
        if (i != n - 1) {
            cur_writer() << commentAfter(ctx->exp()[i + 1], "");
        }
    }
    if (hasIncIndent) {
        decContinuationIndent();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// exp:
// 	NIL
// 	| FALSE
// 	| TRUE
// 	| number
// 	| string
// 	| ELLIPSIS
// 	| prefixexp
// 	| functiondef
// 	| tableconstructor
// 	| exp linkOperator exp
// 	| unaryOperator exp;

antlrcpp::Any FormatVisitor::visitExp(LuaParser::ExpContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (ctx->linkOperator() != nullptr) {
        visitExp(ctx->exp()[0]);
        bool hasIncIndent = false;
        if (config_.get<bool>("break_after_operator")) {
            cur_writer() << commentAfter(ctx->exp()[0], " ");
            cur_writer() << ctx->linkOperator()->getText();
            bool beyondLimit = false;
            pushWriter();
            visitExp(ctx->exp()[1]);
            int length = cur_writer().firstLineColumn();
            length++;  // calc the white space after operator
            popWriter();
            beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
            if (beyondLimit) {
                cur_writer() << commentAfterNewLine(ctx->linkOperator(), INC_CONTINUATION_INDENT);
                cur_writer() << indentWithAlign();
                hasIncIndent = true;
            } else {
                cur_writer() << commentAfter(ctx->linkOperator(), " ");
            }
            visitExp(ctx->exp()[1]);
        } else {
            bool beyondLimit = false;
            pushWriter();
            cur_writer() << ctx->linkOperator()->getText();
            cur_writer() << commentAfter(ctx->linkOperator(), " ");
            visitExp(ctx->exp()[1]);
            int length = cur_writer().firstLineColumn();
            popWriter();
            beyondLimit = cur_columns() + length > config_.get<int>("column_limit");
            if (beyondLimit) {
                cur_writer() << commentAfterNewLine(ctx->exp()[0], INC_CONTINUATION_INDENT);
                cur_writer() << indentWithAlign();
                hasIncIndent = true;
            } else {
                cur_writer() << commentAfter(ctx->exp()[0], " ");
            }
            cur_writer() << ctx->linkOperator()->getText();
            cur_writer() << commentAfter(ctx->linkOperator(), " ");
            visitExp(ctx->exp()[1]);
        }
        if (hasIncIndent) {
            decContinuationIndent();
        }
    } else if (ctx->unaryOperator() != nullptr) {
        cur_writer() << ctx->unaryOperator()->getText();
        if (ctx->unaryOperator()->getText() == "not") {
            cur_writer() << commentAfter(ctx->unaryOperator(), " ");
        } else {
            if (ctx->unaryOperator()->getText() == "-" && ctx->exp().front()->getText()[0] == '-') {
                cur_writer() << commentAfter(ctx->unaryOperator(), " ");
            } else {
                cur_writer() << commentAfter(ctx->unaryOperator(), "");
            }
        }
        visitExp(ctx->exp().front());
    } else if (ctx->prefixexp() != nullptr) {
        visitPrefixexp(ctx->prefixexp());
    } else if (ctx->functiondef() != nullptr) {
        visitFunctiondef(ctx->functiondef());
    } else if (ctx->tableconstructor() != nullptr) {
        visitTableconstructor(ctx->tableconstructor());
    } else if (ctx->string() != nullptr) {
        visitString(ctx->string());
    } else {
        cur_writer() << ctx->getText();
    }
    LOG_FUNCTION_END();
    return nullptr;
}

// varOrExp nameAndArgs*;
antlrcpp::Any FormatVisitor::visitPrefixexp(LuaParser::PrefixexpContext* ctx) {
    LOG_FUNCTION_BEGIN();
    chainedMethodCallHasIncIndent_.push_back(false);
    chainedMethodCallIsFirst_.push_back(false);
    visitVarOrExp(ctx->varOrExp());

    std::string ws = buildFirstArgumentWs(ctx->nameAndArgs());
    if (!ctx->nameAndArgs().empty()) {
        cur_writer() << commentAfter(ctx->varOrExp(), ws);
    }

    if (config_.get<bool>("align_parameter")) {
        buildArguments(ctx->nameAndArgs());
    } else {
        int temp = indentForAlign_;
        indentForAlign_ = 0;
        buildArguments(ctx->nameAndArgs());
        indentForAlign_ = temp;
    }

    if (chainedMethodCallHasIncIndent_.back()) {
        decContinuationIndent();
    }
    chainedMethodCallHasIncIndent_.pop_back();
    chainedMethodCallIsFirst_.pop_back();
    LOG_FUNCTION_END();
    return nullptr;
}

// var | LP exp RP;
antlrcpp::Any FormatVisitor::visitVarOrExp(LuaParser::VarOrExpContext* ctx) {
    LOG_FUNCTION_BEGIN();
    if (ctx->exp() != nullptr) {
        cur_writer() << ctx->LP()->getText();
        cur_writer() << commentAfter(ctx->LP(), "");
        visitExp(ctx->exp());
        cur_writer() << commentAfter(ctx->exp(), "");
        cur_writer() << ctx->RP()->getText();

    } else {
        visitVar(ctx->var());
    }
    LOG_FUNCTION_END();
    return nullptr;
}
