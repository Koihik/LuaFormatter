#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "lua-format.h"

#define TEST_FILE(file)                                                                 \
    TEST_CASE("format file " + std::string(file) + " works well", "format_file") {      \
        std::string filename(file);                                                     \
        std::ifstream input;                                                            \
        input.open(filename);                                                           \
        Config config;                                                                  \
        REQUIRE_THROWS_WITH(lua_format(input, config), "Input contains syntax errors"); \
    }

TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-55.lua");
