#include <gtest/gtest.h>

#include "lua-format.h"

TEST(format, varDecl) {
    EXPECT_EQ("local x, y, z = 1, 2, 3\n", lua_format("local x,y,z=1,2,3"));
    EXPECT_EQ("a, b, c = {}, 1, \"2\"\n", lua_format("a,b,c = {},1,\"2\""));
}

TEST(format, functionCall) {
    EXPECT_EQ("(function()\n    return f1()\nend)():a():b().c(c)\n", lua_format("(function() return f1(); end)():a():b().c(c)"));
}