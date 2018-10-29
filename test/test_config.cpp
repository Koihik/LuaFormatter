#include "catch2/catch.hpp"
#include "lua-format.h"

TEST_CASE("indent", "config") {
    Config config;
    config.indent("  ");
    REQUIRE("function W()\n  print(1)\n  print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));

    config.indent(" ");
    REQUIRE("function W()\n print(1)\n print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));
}

TEST_CASE("table_sep", "config") {
    Config config;
    config.indent("  ");
    config.table_sep(",");

    REQUIRE("x = {\n  1,\n  2,\n  3\n}\n" == lua_format("x = {1,2;3}", config));

    config.table_sep(";");
    REQUIRE("x = {\n  1;\n  2;\n  3\n}\n" == lua_format("x = {1,2;3}", config));
}

TEST_CASE("extra_sep_at_table_end", "config") {
    Config config;
    config.indent("  ");
    config.extra_sep_at_table_end(true);

    REQUIRE("x = {\n  1,\n  2,\n  3,\n}\n" == lua_format("x = {1,2;3}", config));

    config.extra_sep_at_table_end(false);
    REQUIRE("x = {\n  1,\n  2,\n  3\n}\n" == lua_format("x = {1,2;3}", config));
}

TEST_CASE("keep_simple_function_one_line", "config") {
    Config config;
    config.indent("  ");
    config.keep_simple_function_one_line(true);

    REQUIRE("function x() end\n" == lua_format("function x()end", config));
    REQUIRE("function x() print(1) end\n" == lua_format("function x() print(1) end", config));
    // more than one statement
    REQUIRE("function x()\n  print(1)\n  print(2)\nend\n" == lua_format("function x() print(1) print(2) end", config));
    // comment contains line break
    REQUIRE("function x()\n  -- line break\nend\n" == lua_format("function x() \n-- line break\n end", config));

    config.keep_simple_function_one_line(false);
    REQUIRE("function x()\nend\n" == lua_format("function x()end", config));
    REQUIRE("function x()\n  print(1)\nend\n" == lua_format("function x() print(1) end", config));
}

TEST_CASE("keep_simple_table_one_line", "config") {
    Config config;
    config.indent("  ");
    config.keep_simple_table_one_line(true);

    REQUIRE("x = { var1 }\n" == lua_format("x = {var1}", config));
    REQUIRE("x = { func_call:func() }\n" == lua_format("x = {\nfunc_call:func()\n}", config));
    // more than one field
    REQUIRE("x = {\n  1,\n  2,\n  3\n}\n" == lua_format("x = {1,2,3}", config));
    // comment contains line break
    REQUIRE("x = {\n  -- line break\n}\n" == lua_format("x = {\n-- line break\n}", config));

    config.keep_simple_table_one_line(false);
    REQUIRE("x = {\n  var1\n}\n" == lua_format("x = {var1}", config));
    REQUIRE("x = {\n  func_call:func()\n}\n" == lua_format("x = {\nfunc_call:func()\n}", config));
}

TEST_CASE("read from file", "config") {
    Config config;
    config.readFromFile("test/testconfig/1.lua-format");

    REQUIRE("  " == config.indent());
    REQUIRE(";" == config.table_sep());
    REQUIRE(false == config.extra_sep_at_table_end());
    REQUIRE(true == config.keep_simple_function_one_line());
    REQUIRE(true == config.keep_simple_table_one_line());

    config.readFromFile("test/testconfig/2.lua-format");

    REQUIRE("    " == config.indent());
    REQUIRE("," == config.table_sep());
    REQUIRE(false == config.extra_sep_at_table_end());
    REQUIRE(true == config.keep_simple_function_one_line());
    REQUIRE(true == config.keep_simple_table_one_line());
}