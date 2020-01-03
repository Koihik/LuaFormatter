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
        REQUIRE_THROWS_WITH(lua_format(input, config), "Input contains syntax errors");                     \
    }

TEST_FILE(PROJECT_PATH "/test/testdata/issues/issue-55.lua");
