#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "lua-format.h"

#define TEST_FILE(tag, file)                                                              \
    TEST_CASE(std::string(tag) + "/" + std::string(file), "[" + std::string(tag) + "]") { \
        std::string filepath(PROJECT_PATH "/test/lua/");                                   \
        std::ifstream input;                                                              \
        std::string filename = filepath + tag + "/" + file + ".lua";                      \
        std::string expectFileName = filepath + tag + "/_" + file + ".lua";               \
        std::string configFileName = filepath + tag + "/" + file + ".config";             \
        input.open(filename);                                                             \
        Config config;                                                                    \
        if (fs::exists(configFileName)) {                                                 \
            config.readFromFile(configFileName);                                          \
        }                                                                                 \
        std::string actual = lua_format(input, config);                                   \
        std::ifstream expectFile(expectFileName);                                         \
        std::stringstream ss;                                                             \
        ss << expectFile.rdbuf();                                                         \
        std::string expect = ss.str();                                                    \
        REQUIRE(expect == actual);                                                        \
        std::string formatTwice = lua_format(actual, config);                             \
        REQUIRE(expect == formatTwice);                                                   \
    }

TEST_FILE("linebreak", "args_length");
TEST_FILE("linebreak", "chained_call_args");
TEST_FILE("linebreak", "disable_align_in_function");
TEST_FILE("linebreak", "functioncall");
TEST_FILE("linebreak", "functiondef");
TEST_FILE("linebreak", "indent_in_explist");
TEST_FILE("linebreak", "long_var");
TEST_FILE("linebreak", "nested_method_call");
TEST_FILE("linebreak", "operators");
TEST_FILE("linebreak", "table");

TEST_FILE("comment", "function");
TEST_FILE("comment", "space");
TEST_FILE("comment", "table");
TEST_FILE("comment", "varlist");
TEST_FILE("comment", "attrib");
TEST_FILE("comment", "in_args");

TEST_FILE("statement", "function_call");
TEST_FILE("statement", "function");
TEST_FILE("statement", "operator");
TEST_FILE("statement", "semi");
TEST_FILE("statement", "shebang");
TEST_FILE("statement", "statements");
TEST_FILE("statement", "table");
TEST_FILE("statement", "inner_spaces");

TEST_FILE("literals", "doublequote");
TEST_FILE("literals", "singlequote");

TEST_FILE("syntax", "lua54");

TEST_FILE("expression", "function_1");
TEST_FILE("expression", "function_2");

TEST_FILE("disable_format", "simple");
TEST_FILE("disable_format", "with_indent");

TEST_FILE("keep_simple_block_one_line", "default");
TEST_FILE("keep_simple_block_one_line", "keep_simple_function_one_line_false");
TEST_FILE("keep_simple_block_one_line", "keep_simple_control_block_one_line_false");

TEST_FILE("issues", "issue-1");
TEST_FILE("issues", "issue-18");
TEST_FILE("issues", "issue-19");
TEST_FILE("issues", "issue-36");
TEST_FILE("issues", "issue-62_1");
TEST_FILE("issues", "issue-62_2");
TEST_FILE("issues", "issue-62_3");
TEST_FILE("issues", "issue-70");
TEST_FILE("issues", "issue-98");
TEST_FILE("issues", "issue-98_1");
TEST_FILE("issues", "issue-80");
TEST_FILE("issues", "issue-104");
TEST_FILE("issues", "issue-156");
TEST_FILE("issues", "issue-162");
TEST_FILE("issues", "issue-168");
TEST_FILE("issues", "issue-186");
TEST_FILE("issues", "PR-100");
TEST_FILE("issues", "PR-108");
TEST_FILE("issues", "PR-181");