#include "visitor/FormatVisitor.h"

#include <iostream>
#include <iterator>
#include <regex>
#include <utility>

#include "LuaLexer.h"

using namespace antlr4;

static bool isBlankChar(const char c) { return c == ' ' || c == '\t' || c == '\u000C'; }

int firstNodeIndex(tree::ParseTree* t) {
    if (t == nullptr) {
        return -1;
    }
    auto* c = dynamic_cast<ParserRuleContext*>(t);
    if (c != nullptr) {
        return c->getStart()->getTokenIndex();
    }
    auto* n = dynamic_cast<tree::TerminalNode*>(t);
    if (n != nullptr) {
        return n->getSymbol()->getTokenIndex();
    }
    return INT_MAX;
}

int lastNodeIndex(tree::ParseTree* t) {
    if (t == nullptr) {
        return -1;
    }
    auto* c = dynamic_cast<ParserRuleContext*>(t);
    if (c != nullptr) {
        return c->getStop()->getTokenIndex();
    }
    auto* n = dynamic_cast<tree::TerminalNode*>(t);
    if (n != nullptr) {
        return n->getSymbol()->getTokenIndex();
    }
    return INT_MAX;
}

// commentAfter returns comments between a ParseTreeNode and the next Node.
// expect: return expect string if no comments found
std::string FormatVisitor::commentAfter(tree::ParseTree* node, const std::string& expect) {
    int l = lastNodeIndex(node);
    std::stringstream ss;
    // No space on left of first comment
    bool lastComment = node != nullptr;
    for (unsigned int i = l + 1; i < tokens_.size(); i++) {
        auto* token = tokens_[i];
        if (token->getType() == LuaLexer::COMMENT) {
            if (lastComment) {
                ss << " ";
            }
            ss << token->getText();
            lastComment = true;
        } else if (token->getType() == LuaLexer::LINE_COMMENT) {
            if (lastComment) {
                ss << " ";
            }
            ss << formatLineComment(token);
            ss << indent();
            lastComment = false;
        } else if (token->getType() == LuaLexer::SHEBANG) {
            ss << token->getText() << "\n\n";
        } else if (token->getType() == LuaLexer::WS) {
        } else {
            break;
        }
    }
    if (ss.tellp() == 0) {
        ss << expect;
    } else if (lastComment) {
        ss << " ";
    }
    return ss.str();
}

// commentAfterNewLine returns comments between a ParseTreeNode and the next Node.
// This function always expect a line break.
// NewLineIndent: inc or dec indent after a line break.
std::string FormatVisitor::commentAfterNewLine(tree::ParseTree* node, NewLineIndent newLineIndent) {
    auto* ctx = dynamic_cast<ParserRuleContext*>(node);
    if (ctx != nullptr && ctx->children.empty()) {
        if (newLineIndent == INC_INDENT) {
            incIndent();
        } else if (newLineIndent == INC_CONTINUATION_INDENT) {
            incContinuationIndent();
        } else if (newLineIndent == DEC_INDENT) {
            decIndent();
        } else if (newLineIndent == DEC_CONTINUATION_INDENT) {
            decContinuationIndent();
        }
        return "";
    }
    int l = lastNodeIndex(node);
    if (newLineIndent == INC_INDENT) {
        incIndent();
    } else if (newLineIndent == INC_CONTINUATION_INDENT) {
        incContinuationIndent();
    }
    std::stringstream ss;
    bool customNewLine = false;
    bool lastComment = true;
    bool lastestNewLine = false;
    for (unsigned int i = l + 1; i < tokens_.size(); i++) {
        auto* token = tokens_[i];
        if (token->getType() == LuaLexer::COMMENT) {
            if (lastestNewLine) {
                ss << indent();
            } else if (lastComment) {
                ss << " ";
            }
            ss << token->getText();
            lastComment = true;
            lastestNewLine = false;
        } else if (token->getType() == LuaLexer::LINE_COMMENT) {
            if (lastestNewLine) {
                ss << indent();
            } else if (lastComment) {
                ss << " ";
            }
            ss << formatLineComment(token);
            customNewLine = true;
            lastComment = false;
            lastestNewLine = true;
        } else if (token->getType() == LuaLexer::WS) {
            auto ln = token->getText().find('\n');
            auto rn = token->getText().rfind('\n');
            if (ln != std::string::npos) {
                if (ln != rn && !lastestNewLine) {
                    ss << "\n\n";
                } else {
                    ss << "\n";
                }
                customNewLine = true;
                lastComment = false;
                lastestNewLine = true;
            }
        } else {
            break;
        }
    }
    if (!customNewLine) {
        ss << "\n";
        lastestNewLine = true;
    }
    if (newLineIndent == DEC_INDENT) {
        decIndent();
    } else if (newLineIndent == DEC_CONTINUATION_INDENT) {
        decContinuationIndent();
    }
    if (lastComment && !lastestNewLine) {
        ss << "\n";
    }
    return ss.str();
}

void FormatVisitor::pushWriter() {
    SourceWriter* writer = new SourceWriter(0);
    if (!writers_.empty()) {
        writer->set_volatile(true);
    }
    writers_.push_back(writer);
}

void FormatVisitor::pushWriterWithColumn() {
    SourceWriter* writer = new SourceWriter(cur_columns());
    if (!writers_.empty()) {
        writer->set_volatile(true);
    }
    writers_.push_back(writer);
}

void FormatVisitor::popWriter() {
    SourceWriter* writer = writers_.back();
    writers_.pop_back();
    delete writer;
}

std::pair<int, int> FormatVisitor::calcASTLengthAndLines(tree::ParseTree* node, std::function<void()> func) {
    if (node == nullptr) return {0, 0};
    if (lengthCache_.find(node) != lengthCache_.end()) {
        return lengthCache_[node];
    }
    pushWriterWithColumn();
    func();
    int length = cur_writer().firstLineColumn();
    int lines = cur_writer().lines();
    popWriter();
    std::pair<int, int> ret = std::make_pair(length, lines);
    lengthCache_[node] = ret;
    return ret;
}

bool FormatVisitor::fastTestColumnLimit(tree::ParseTree* node) {
    int columns = cur_columns();
    for (int i = firstNodeIndex(node), n = lastNodeIndex(node); i <= n; i++) {
        if (tokens_[i]->getType() == LuaLexer::COMMENT || tokens_[i]->getType() == LuaLexer::LINE_COMMENT ||
            tokens_[i]->getType() == LuaLexer::WS) {
            continue;
        }
        columns += tokens_[i]->getText().size();
        if (columns > config_.get<int>("column_limit")) {
            return true;
        }
    }
    return false;
}

SourceWriter& FormatVisitor::cur_writer() { return *writers_.back(); }

int FormatVisitor::cur_columns() { return (*writers_.back()).columns().back(); }

// Add a white space before line comment
std::string FormatVisitor::formatLineComment(Token* token) {
    std::string comment = token->getText();
    char firstChar = comment[2];
    if (isBlankChar(firstChar) || firstChar == '-' || firstChar == '\r' || firstChar == '\n') {
        // do nothing
    } else {
        comment = "-- " + comment.substr(2, comment.size());
    }
    return comment;
}

antlrcpp::Any FormatVisitor::visitString(LuaParser::StringContext* ctx) {
    if ((ctx->NORMALSTRING() == nullptr) && (ctx->CHARSTRING() == nullptr)) {
        cur_writer() << ctx->getText();
        return nullptr;
    }
    char quote = ctx->NORMALSTRING() != nullptr ? '\'' : '\"';

    switch (quote) {
        case '\"':
            if (config_.get<bool>("single_quote_to_double_quote")) {
                break;
            }
            cur_writer() << ctx->getText();
            return nullptr;
        case '\'':
            if (config_.get<bool>("double_quote_to_single_quote")) {
                break;
            }
            cur_writer() << ctx->getText();
            return nullptr;
        default:
            assert(0);
    }

    //Switch the type of quote
    std::string newstr = ctx->getText();
    newstr.front() = quote;    
    newstr.back() = quote;
    bool escaped = false;  //Is the current character in the sequence escaped
    const char oldChar = quote == '\'' ? '"' : '\'';  
    auto itr = ++newstr.begin();
    auto itrStop = --newstr.end();
    while (itr != itrStop) {
        if (*itr == '\\') {
            escaped = !escaped;  //If the character isn't currently escaped, the next one will be escaped
        } else {
            if (*itr == quote) {
                // Add the escape character before the non escaped old character
                if (!escaped) {
                    itr = newstr.insert(itr, '\\');
                    ++itr;  // Extra incriment to get back to the previous character
                    itrStop = --newstr.end();
                }
            } else if (*itr == oldChar) {
                // Remove the escape before the previously escaped old character
                if (escaped) {
                    itr = newstr.erase(--itr);
                    itrStop = --newstr.end();
                }        
            }
            escaped = false;
        }
        ++itr;
    }

    cur_writer() << newstr;
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitChildren(tree::ParseTree* node) {
    size_t n = node->children.size();
    for (size_t i = 0; i < n; i++) {
        node->children[i]->accept(this);
    }
    return nullptr;
}

antlrcpp::Any FormatVisitor::visitTerminal(tree::TerminalNode* node) {
    cur_writer() << node->getText();
    return nullptr;
}

bool FormatVisitor::needKeepBlockOneLine(tree::ParseTree* previousNode, LuaParser::BlockContext* ctx,
                                         BlockType blockType) {
    if (blockType == CONTROL_BLOCK && !config_.get<bool>("keep_simple_control_block_one_line")) {
        return false;
    }
    if (blockType == FUNCTION_BLOCK && !config_.get<bool>("keep_simple_function_one_line")) {
        return false;
    }

    int stats = 0;
    for (auto& s : ctx->stat()) {
        if (s->SEMI() == nullptr) {
            stats++;
        }
    }
    if (ctx->retstat() != nullptr) {
        stats++;
    }
    // block contains more than one statement.
    if (stats > 1) {
        return false;
    }

    // block contains a line break.
    std::string preComment = commentAfter(previousNode, "");
    if (preComment.find('\n') != std::string::npos) {
        return false;
    }
    if (fastTestColumnLimit(ctx)) {
        return false;
    }
    pushWriter();
    visitBlock(ctx);
    int length = cur_writer().firstLineColumn();
    int lines = cur_writer().lines();
    popWriter();
    if (cur_columns() + length > config_.get<int>("column_limit") || lines > 1) {
        return false;
    }
    std::string postComment = commentAfter(ctx, "");
    return postComment.find('\n') == std::string::npos;
}

bool FormatVisitor::isBlockEmpty(LuaParser::BlockContext* ctx) {
    return ctx->stat().empty() && ctx->retstat() == nullptr;
}


std::string FormatVisitor::indent() {
    std::stringstream ss;
    for (int i = 0; i < indent_; i++) {
        if (config_.get<bool>("use_tab")) {
            ss << "\t";
        } else {
            ss << " ";
        }
    }
    return ss.str();
}

std::string FormatVisitor::indentWithAlign() {
    std::stringstream ss;
    if (config_.get<bool>("use_tab")) {
        for (int i = 0; i < indent_; i++) {
            ss << "\t";
        }
        for (int i = 0; i < indentForAlign_; i++) {
            ss << " ";
        }
    } else {
        for (int i = 0; i < indent_ + indentForAlign_; i++) {
            ss << " ";
        }
    }
    return ss.str();
}

void FormatVisitor::incIndent() {
    LOG_FUNCTION_BEGIN();
    indent_ += config_.get<int>("indent_width");
    LOG_FUNCTION_END();
}

void FormatVisitor::decIndent() {
    LOG_FUNCTION_BEGIN();
    indent_ -= config_.get<int>("indent_width");
    LOG_FUNCTION_END();
}

void FormatVisitor::incContinuationIndent() {
    LOG_FUNCTION_BEGIN();
    indent_ += config_.get<int>("continuation_indent_width");
    LOG_FUNCTION_END();
}

void FormatVisitor::decContinuationIndent() {
    LOG_FUNCTION_BEGIN();
    indent_ -= config_.get<int>("continuation_indent_width");
    LOG_FUNCTION_END();
}

void FormatVisitor::incIndentForAlign(int indent) {
    LOG_FUNCTION_BEGIN();
    indentForAlign_ += indent;
    LOG_FUNCTION_END();
}

void FormatVisitor::decIndentForAlign(int indent) {
    LOG_FUNCTION_BEGIN();
    indentForAlign_ -= indent;
    LOG_FUNCTION_END();
}
