#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "catch2/catch.hpp"
#include "lua-format.h"

using namespace std;

#define TEST_FILE(file)                                                                                     \
    TEST_CASE("format file " + string(file) + " works well", "format_file") {                               \
        string filename(file);                                                                              \
        ifstream input;                                                                                     \
        input.open(filename);                                                                               \
        Config config;                                                                                      \
        string actul = lua_format(input, config);                                                           \
        int idx = filename.find_last_of('/');                                                               \
        string expectFileName = filename.substr(0, idx) + "/_" + filename.substr(idx + 1, filename.size()); \
        ifstream expectFile(expectFileName);                                                                \
        stringstream ss;                                                                                    \
        ss << expectFile.rdbuf();                                                                           \
        string expect = ss.str();                                                                           \
        REQUIRE(expect == actul);                                                                           \
        string formatTwice = lua_format(actul, config);                                                     \
        REQUIRE(expect == formatTwice);                                                                     \
    }

TEST_FILE("../test/testdata/chop_down/parameter.lua");
TEST_FILE("../test/testdata/chop_down/function.lua");
TEST_FILE("../test/testdata/chop_down/table.lua");

TEST_FILE("../test/testdata/comment/function.lua");
TEST_FILE("../test/testdata/comment/space.lua");
TEST_FILE("../test/testdata/comment/table.lua");
TEST_FILE("../test/testdata/comment/varlist.lua");

TEST_FILE("../test/testdata/statement/function_call.lua");
TEST_FILE("../test/testdata/statement/function.lua");
TEST_FILE("../test/testdata/statement/operator.lua");
TEST_FILE("../test/testdata/statement/semi.lua");
TEST_FILE("../test/testdata/statement/shebang.lua");
TEST_FILE("../test/testdata/statement/statements.lua");
TEST_FILE("../test/testdata/statement/table.lua");
