#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "lua-format.h"

using namespace std;

TEST(format_file, all_file) {
    vector<string> files = {
        "test/testdata/other.lua",
        "test/testdata/comments.lua",
        "test/testdata/table.lua",
    };

    for (string& file : files) {
        ifstream input;
        input.open(file);

        string actul = lua_format(input);

        string expectFileName = file.substr(0, file.size() - 4) + "_out.lua";
        cerr << "file = " << file << endl;
        cerr << "expectFileName = " << expectFileName << endl;

        ifstream expectFile(expectFileName);
        stringstream ss;
        ss << expectFile.rdbuf();

        EXPECT_EQ(ss.str(), actul);
    }
}
