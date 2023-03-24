#include "lua-format.h"

#include <iostream>

#include "visitor/FormatVisitor.h"
#include "LuaLexer.h"

using namespace antlr4;

static std::string __format(ANTLRInputStream& input, const Config& config) {
    LuaLexer lexer(&input);
    CommonTokenStream tokenstream(&lexer);
    LuaParser parser(&tokenstream);

    LuaParser::ChunkContext* chunk = parser.chunk();

    if (parser.getNumberOfSyntaxErrors() > 0) {
        throw std::invalid_argument("Input contains syntax errors");
    }

    std::vector<antlr4::Token*> tokenVector;
    for (auto* t : tokenstream.getTokens()) {
        tokenVector.emplace_back(t);
    }

    FormatVisitor visitor(tokenVector, config);
    return chunk->accept(&visitor).as<std::string>();
}

static const std::string DISABLE_FORMAT_BEGIN = "-- LuaFormatter off";
static const std::string DISABLE_FORMAT_END = "-- LuaFormatter on";

// TODO: Collect it while walking AST seems better
std::vector<std::pair<size_t, size_t>> findBlocksBetweenFormatSwitch(const std::string& str) {
    std::vector<std::pair<size_t, size_t>> blocks;
    size_t pos = std::string::npos;
    size_t findAt = 0;
    std::deque<size_t> startArr;
    while ((pos = str.find(DISABLE_FORMAT_BEGIN, findAt)) != std::string::npos) {
        startArr.push_back(pos);
        findAt = pos + DISABLE_FORMAT_BEGIN.size();
    }
    pos = std::string::npos;
    findAt = 0;
    while ((pos = str.find(DISABLE_FORMAT_END, findAt)) != std::string::npos) {
        if (!startArr.empty()) {
            size_t start = startArr.front();
            startArr.pop_front();
            blocks.emplace_back(start, pos);
        }
        findAt = pos + DISABLE_FORMAT_END.size();
    }
    return blocks;
}

static std::string getOSLineSeparator() {
#ifdef _WIN32
    return "\r\n";
#else
    return "\n";
#endif
}

std::string handleLineSeparator(const std::string& original, const std::string& formatted, const Config& config) {
    const auto line_separator_config = config.get<std::string>("line_separator");
    auto out = formatted;
    if (line_separator_config == "os") {
        out = convert_line_separator(out, getOSLineSeparator());
    } else if (line_separator_config == "input") {
        auto line_sep = get_line_separator(original);
        out = convert_line_separator(out, line_sep);
    } else if (line_separator_config == "lf") {
        out = convert_line_separator(out, "\n");
    } else if (line_separator_config == "cr") {
        out = convert_line_separator(out, "\r");
    } else if (line_separator_config == "crlf") {
        out = convert_line_separator(out, "\r\n");
    } else {
        throw std::runtime_error("Should not reach here. Invalid line_separator config");
    }
    return out;
}

std::string resetContentInDisableFormatBlocks(const std::string& original, const std::string& formatted) {
    std::vector<std::pair<size_t, size_t>> originalBlocks = findBlocksBetweenFormatSwitch(original);
    std::vector<std::pair<size_t, size_t>> formattedBlocks = findBlocksBetweenFormatSwitch(formatted);
    if (originalBlocks.size() != formattedBlocks.size()) {
        std::cerr << "Unexpected! originalBlocks.size() = " << originalBlocks.size()
                  << " , formattedBlocks.size() = " << formattedBlocks.size() << std::endl;
    }
    size_t sz = std::min(originalBlocks.size(), formattedBlocks.size());
    if (sz == 0) {
        return formatted;
    }
    std::ostringstream os;
    os << formatted.substr(0, formattedBlocks[0].first);
    for (size_t i = 0; i < sz; i++) {
        size_t startAt = originalBlocks[i].first;
        size_t endAt = originalBlocks[i].second;
        size_t nextFormttedBlockStartAt = formattedBlocks[i].second;
        size_t nextFormttedBlockEndAt = formatted.size();
        if (i != sz - 1) {
            nextFormttedBlockEndAt = formattedBlocks[i + 1].first;
        }
        os << original.substr(startAt, endAt - startAt);
        os << formatted.substr(nextFormttedBlockStartAt, nextFormttedBlockEndAt - nextFormttedBlockStartAt);
    }
    return os.str();
}

std::string lua_format(std::istream& is, const Config& config) {
    std::ostringstream os;
    os << is.rdbuf();
    std::string original = os.str();
    ANTLRInputStream input(original);
    std::string formatted = __format(input, config);
    formatted = handleLineSeparator(original, formatted, config);
    return resetContentInDisableFormatBlocks(original, formatted);
}

std::string lua_format(const std::string& str, const Config& config) {
    ANTLRInputStream input(str);
    std::string formatted = __format(input, config);
    formatted = handleLineSeparator(str, formatted, config);
    return resetContentInDisableFormatBlocks(str, formatted);
}

std::string get_line_separator(const std::string& input) {
    constexpr char CR = '\r';
    constexpr char LF = '\n';
    size_t lf_count = 0;
    size_t cr_count = 0;
    size_t crlf_count = 0;
    const auto length = input.length();
    for (size_t i = 0; i < length; i++) {
        const auto cur = input[i];
        if (cur == LF) {
            lf_count++;
        } else if (cur == CR) {
            const auto next = input[i + 1];
            if (next == LF) {
                crlf_count++;
                i++;
            } else {
                cr_count++;
            }
        }
    }

    std::string result;
    if (lf_count >= crlf_count && lf_count >= cr_count) {
        result += LF;
    } else if (crlf_count >= lf_count && crlf_count >= cr_count) {
        result += CR;
        result += LF;
    } else {
        result += CR;
    }
    return result;
}

std::string convert_line_separator(const std::string& input, const std::string& line_sep) {
    constexpr char CR = '\r';
    constexpr char LF = '\n';
    const auto length = input.length();

    std::string result;
    for (size_t i = 0; i < length; i++) {
        const auto cur = input[i];
        if (cur == LF) {
            result += line_sep;
        } else if (cur == CR) {
            const auto next = input[i + 1];
            if (next == LF) {
                result += line_sep;
                i++;
            } else {
                result += line_sep;
            }
        } else {
            result += cur;
        }
    }
    return result;
}
