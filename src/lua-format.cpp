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

string lua_format(istream& is, const Config& config) {
    ANTLRInputStream input(is);
    return __format(input, config);
}

string lua_format(const string& str, const Config& config) {
    ANTLRInputStream input(str);
    return __format(input, config);
}
