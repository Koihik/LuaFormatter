#include <iostream>

#include "FormatVisitor.h"
#include "LuaLexer.h"

using namespace std;
using namespace antlr4;

static string __format(ANTLRInputStream& input, const Config& config) {
    LuaLexer lexer(&input);
    CommonTokenStream tokenstream(&lexer);
    LuaParser parser(&tokenstream);

    LuaParser::ChunkContext* chunk = parser.chunk();

    if (parser.getNumberOfSyntaxErrors() > 0) {
        throw invalid_argument("Input contains syntax errors");
    }

    vector<antlr4::Token*> tokenVector;
    for (auto t : tokenstream.getTokens()) {
        tokenVector.emplace_back(t);
    }

    FormatVisitor visitor(tokenVector, config);
    return chunk->accept(&visitor).as<string>();
}

static const string DISABLE_FORMAT_BEGIN = "-- LuaFormatter off";
static const string DISABLE_FORMAT_END = "-- LuaFormatter on";

// TODO: Collect it while walking AST seems better
vector<pair<size_t, size_t>> findBlocksBetweenFormatSwitch(const string& str) {
    vector<pair<size_t, size_t>> blocks;
    size_t pos = string::npos;
    size_t findAt = 0;
    deque<size_t> startArr;
    while ((pos = str.find(DISABLE_FORMAT_BEGIN, findAt)) != string::npos) {
        startArr.push_back(pos);
        findAt = pos + DISABLE_FORMAT_BEGIN.size();
    }
    pos = string::npos;
    findAt = 0;
    while ((pos = str.find(DISABLE_FORMAT_END, findAt)) != string::npos) {
        if (!startArr.empty()) {
            size_t start = startArr.front();
            startArr.pop_front();
            blocks.push_back({start, pos});
        }
        findAt = pos + DISABLE_FORMAT_END.size();
    }
    return blocks;
}

string resetContentInDisableFormatBlocks(const string& original, const string& formatted) {
    vector<pair<size_t, size_t>> originalBlocks = findBlocksBetweenFormatSwitch(original);
    vector<pair<size_t, size_t>> formattedBlocks = findBlocksBetweenFormatSwitch(formatted);
    if (originalBlocks.size() != formattedBlocks.size()) {
        cerr << "Unexpected! originalBlocks.size() = " << originalBlocks.size()
             << " , formattedBlocks.size() = " << formattedBlocks.size() << endl;
    }
    size_t sz = min(originalBlocks.size(), formattedBlocks.size());
    if (sz == 0) {
        return formatted;
    }
    ostringstream os;
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

string lua_format(istream& is, const Config& config) {
    std::ostringstream os;
    os << is.rdbuf();
    string original = os.str();
    ANTLRInputStream input(original);
    string formatted = __format(input, config);
    return resetContentInDisableFormatBlocks(original, formatted);
}

string lua_format(const string& str, const Config& config) {
    ANTLRInputStream input(str);
    string formatted = __format(input, config);
    return resetContentInDisableFormatBlocks(str, formatted);
}
