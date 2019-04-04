#include <iostream>

#include "FormatVisitor.h"
#include "LuaLexer.h"

using namespace std;
using namespace antlr4;

string __foramt(ANTLRInputStream& input, const Config& config) {
    LuaLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    LuaParser parser(&tokens);

    LuaParser::ChunkContext* chunk = parser.chunk();

    if (parser.getNumberOfSyntaxErrors() > 0) {
        throw invalid_argument("Input contains syntax errors");
    }

    vector<antlr4::Token*> tokenVector;
    for (auto t : tokens.getTokens()) {
        tokenVector.emplace_back(t);
    }

    FormatVisitor visitor(tokenVector, config);
    return chunk->accept(&visitor).as<string>();
}

string lua_format(istream& is, const Config& config) {
    ANTLRInputStream input(is);
    return __foramt(input, config);
}

string lua_format(const string& str, const Config& config) {
    ANTLRInputStream input(str);
    return __foramt(input, config);
}
