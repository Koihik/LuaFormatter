#include "catch2/catch.hpp"
#include "lua-format.h"

TEST_CASE("format varDecl works", "format") {
    REQUIRE("local x, y, z = 1, 2, 3\n" == lua_format("local x,y,z=1,2,3"));
    REQUIRE("a, b, c = {}, 1, \"2\"\n" == lua_format("a,b,c = {},1,\"2\""));
}

TEST_CASE("format functionCall works", "format") {
    REQUIRE("(function()\n    return f1()\nend)():a():b().c(c)\n" == lua_format("(function() return f1(); end)():a():b().c(c)"));
}