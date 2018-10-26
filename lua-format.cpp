#include <iostream>

#include "FormatVisitor.h"
#include "LuaLexer.h"

using namespace std;
using namespace antlr4;

string format(string filePath) {
    std::ifstream stream;
    stream.open(filePath);

    ANTLRInputStream input(stream);
    LuaLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    LuaParser parser(&tokens);

    LuaParser::ChunkContext* chunk = parser.chunk();

    FormatVisitor visitor(tokens.getTokens());
    string out = chunk->accept(&visitor);
    return out;
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        cerr << "No input file specified." << endl;
        return -1;
    }
    if (argc > 2) {
        cerr << "More than one file specified." << endl;
        return -1;
    }
    cout << format(argv[1]);
    return 0;
}