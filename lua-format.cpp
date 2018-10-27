#include <iostream>

#include "FormatVisitor.h"
#include "LuaLexer.h"

using namespace std;
using namespace antlr4;

string __foramt(ANTLRInputStream& input) {
    LuaLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    LuaParser parser(&tokens);

    LuaParser::ChunkContext* chunk = parser.chunk();

    vector<antlr4::Token*> tokenVector;
    for (auto t : tokens.getTokens()) {
        tokenVector.emplace_back(t);
    }

    FormatVisitor visitor(tokenVector);
    string out = chunk->accept(&visitor);
    return out;
}

string lua_format(istream& is) {
    ANTLRInputStream input(is);
    return __foramt(input);
}

string lua_format(const string& str) {
    ANTLRInputStream input(str);
    return __foramt(input);
}
