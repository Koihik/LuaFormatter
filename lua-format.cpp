#include <iostream>

#include "FormatVisitor.h"
#include "antlr4-runtime/LuaLexer.h"
#include "antlr4-runtime/LuaParser.h"
#include "antlr4-runtime/LuaVisitor.h"
#include "antlr4-runtime/antlr4-runtime.h"

using namespace std;
using namespace antlr4;

int main(int argc, const char* argv[]) {
    std::ifstream stream;
    stream.open("testdata/t1.lua");

    ANTLRInputStream input(stream);
    LuaLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    LuaParser parser(&tokens);

    LuaParser::ChunkContext* chunk = parser.chunk();

    // std::ifstream commentStream;
    // commentStream.open("testdata/t1.lua");
    // ANTLRInputStream cinput(commentStream);
    // LuaLexer commentLexer(&cinput);
    // CommonTokenStream commentTokenStream(&commentLexer, 1);
    // commentTokenStream.fill();

    FormatVisitor visitor(tokens.getTokens());

    string out = chunk->accept(&visitor);

    cout << out;

    return 0;
}