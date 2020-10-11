#include <iostream>

#include "FormatVisitor.h"
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
    return resetContentInDisableFormatBlocks(original, formatted);
}

std::string lua_format(const std::string& str, const Config& config) {
    ANTLRInputStream input(str);
    std::string formatted = __format(input, config);
    return resetContentInDisableFormatBlocks(str, formatted);
}
