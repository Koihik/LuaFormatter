#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "catch2/catch.hpp"
#include "lua-format.h"

using namespace std;

#define TEST_FILE(file)                                                               \
    TEST_CASE("format file " + string(file) + " works well", "format_file") {         \
        string filename(file);                                                        \
        ifstream input;                                                               \
        input.open(filename);                                                         \
        string actul = lua_format(input);                                             \
        string expectFileName = filename.substr(0, filename.size() - 4) + "_out.lua"; \
        ifstream expectFile(expectFileName);                                          \
        stringstream ss;                                                              \
        ss << expectFile.rdbuf();                                                     \
        string expect = ss.str();                                                     \
        REQUIRE(expect == actul);                                                     \
    }

TEST_FILE("test/testdata/other.lua");
TEST_FILE("test/testdata/comments.lua");
TEST_FILE("test/testdata/table.lua");
TEST_FILE("test/testdata/semi.lua");
TEST_FILE("test/testdata/statements.lua");
