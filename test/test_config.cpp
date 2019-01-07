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

    REQUIRE("x = {1, 2, 3}\n" == lua_format("x = {1,2;3}", config));

    config.table_sep(";");
    REQUIRE("x = {1; 2; 3}\n" == lua_format("x = {1,2;3}", config));
}

TEST_CASE("extra_sep_at_table_end", "config") {
    Config config;
    config.indent("  ");
    config.extra_sep_at_table_end(true);

    REQUIRE("x = {1, 2, 3}\n" == lua_format("x = {1,2;3}", config));

    REQUIRE("x = {\n  1, -- line break\n  2,\n  3,\n}\n" == lua_format("x = {1,-- line break\n2;3}", config));
    config.extra_sep_at_table_end(false);
    REQUIRE("x = {\n  1, -- line break\n  2,\n  3\n}\n" == lua_format("x = {1,-- line break\n2;3}", config));
}

TEST_CASE("chop_down_parameter", "config") {
    Config config;
    config.indent("  ");

    REQUIRE("call(1, 2, 3)\n" == lua_format("call(1,2,3)", config));

    config.chop_down_parameter(5);
    REQUIRE("call(\n  1,\n  2,\n  3\n)\n" == lua_format("call(1,2,3)", config));
}

TEST_CASE("chop_down_block", "config") {
    Config config;
    config.indent("  ");

    REQUIRE("function a() print(1) end\n" == lua_format("function a() print(1) end", config));

    config.chop_down_block(5);
    REQUIRE("function a()\n  print(1)\nend\n" == lua_format("function a() print(1) end", config));
}

TEST_CASE("chop_down_table", "config") {
    Config config;
    config.indent("  ");

    REQUIRE("x = {1, 2, 3}\n" == lua_format("x = {1,2,3}", config));

    config.chop_down_table(5);
    REQUIRE("x = {\n  1,\n  2,\n  3\n}\n" == lua_format("x = {1,2,3}", config));
}

TEST_CASE("keep_simple_block_one_line", "config") {
    Config config;
    config.indent("  ");

    REQUIRE("function x() print(1) end\n" == lua_format("function x() print(1) end", config));

    config.keep_simple_block_one_line(false);
    REQUIRE("function x()\n  print(1)\nend\n" == lua_format("function x() print(1) end", config));
}

TEST_CASE("read from file", "config") {
    Config config;
    config.readFromFile("../test/testconfig/1.lua-format");

    REQUIRE("  " == config.indent());
    REQUIRE(";" == config.table_sep());
    REQUIRE(false == config.extra_sep_at_table_end());

    config.readFromFile("../test/testconfig/2.lua-format");

    REQUIRE("    " == config.indent());
    REQUIRE("," == config.table_sep());
    REQUIRE(false == config.extra_sep_at_table_end());
}