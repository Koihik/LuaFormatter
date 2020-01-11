#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <catch2/catch.hpp>
#include "lua-format.h"

using namespace std;
namespace fs = filesystem;

#define TEST_FILE(file)                                                                                     \
    TEST_CASE("format file " + string(file) + " works well", "format_file") {                               \
        string filename(file);                                                                              \
        ifstream input;                                                                                     \
        input.open(filename);                                                                               \
        Config config;                                                                                      \
        int idx = filename.find_last_of('/');                                                               \
        string expectFileName = filename.substr(0, idx) + "/_" + filename.substr(idx + 1, filename.size()); \
        idx = filename.find_last_of('.');                                                                   \
        string configFileName = filename.substr(0, idx) + ".config";                                        \
        if (fs::exists(configFileName)) {                                                                   \
            std::cout << configFileName << " exist" << endl;                                                \
            config.readFromFile(configFileName);                                                            \
            std::cout << config.get<bool>("chop_down_parameter") << " dd" << endl;                          \
        }                                                                                                   \
        string actual = lua_format(input, config);                                                          \
        ifstream expectFile(expectFileName);                                                                \
        stringstream ss;                                                                                    \
        ss << expectFile.rdbuf();                                                                           \
        string expect = ss.str();                                                                           \
        REQUIRE(expect == actual);                                                                          \
        string formatTwice = lua_format(actual, config);                                                    \
        REQUIRE(expect == formatTwice);                                                                     \
    }

TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/args_length.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/block.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/chained_call_args.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/disable_align_in_function.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/functioncall.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/functiondef.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/indent_in_explist.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/long_var.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/nested_method_call.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/operators.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/linebreak/table.lua");

TEST_FILE(PROJECT_PATH "/test/testdata/comment/function.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/comment/space.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/comment/table.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/comment/varlist.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/comment/attrib.lua");

TEST_FILE(PROJECT_PATH "/test/testdata/statement/function_call.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/statement/function.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/statement/operator.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/statement/semi.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/statement/shebang.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/statement/statements.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/statement/table.lua");

TEST_FILE(PROJECT_PATH "/test/testdata/literals/doublequote.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/literals/singlequote.lua");

TEST_FILE(PROJECT_PATH "/test/testdata/syntax/lua54.lua");

TEST_FILE(PROJECT_PATH "/test/testdata/expression/function_1.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/expression/function_2.lua");

TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-1.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-18.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-19.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-36.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-62_1.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-62_2.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-62_3.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-70.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-80.lua");

TEST_FILE(PROJECT_PATH "/test/testdata/keep_simple_block_one_line/default.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/keep_simple_block_one_line/keep_simple_function_one_line_false.lua");
TEST_FILE(PROJECT_PATH "/test/testdata/keep_simple_block_one_line/keep_simple_control_block_one_line_false.lua");

