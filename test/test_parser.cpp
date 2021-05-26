#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "lua-format.h"

TEST_CASE("wrong shebang", "[parser]") {
    Config config;
    REQUIRE_THROWS_WITH(lua_format(
R"(local foo = "aaa"
#!/bin/lua5.3
local foo = "a"
)", config), "Input contains syntax errors");
}
