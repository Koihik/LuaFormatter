#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "catch2/catch.hpp"
#include "lua-format.h"

using namespace std;

TEST_CASE("format_file works well", "format_file") {
    vector<string> files = {
        "test/testdata/other.lua",
        "test/testdata/comments.lua",
        "test/testdata/table.lua",
        "test/testdata/semi.lua",
    };

    for (string& file : files) {
        ifstream input;
        input.open(file);

        string actul = lua_format(input);

        string expectFileName = file.substr(0, file.size() - 4) + "_out.lua";

        ifstream expectFile(expectFileName);
        stringstream ss;
        ss << expectFile.rdbuf();

        string expect = ss.str();

        REQUIRE(expect == actul);

        // FIXME: single semi in comments bug
        // string formatTwice = lua_format(actul);
        // REQUIRE(expect == formatTwice);
    }
}
