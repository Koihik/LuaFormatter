#include "catch2/catch.hpp"
#include "lua-format.h"

TEST_CASE("indent", "config") {
    Config config;
    config.indent_width(2);
    REQUIRE("function W()\n  print(1)\n  print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));

    config.indent_width(1);
    REQUIRE("function W()\n print(1)\n print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));
}

TEST_CASE("column_limit", "config") {
    Config config;
    config.indent_width(1);
    config.continuation_indent_width(2);
    config.column_limit(10);
    REQUIRE("function W()\n q(aa,\n   bbb)\n print(\n aaaa,\n bbb)\nend\n" == lua_format("function W() q(aa,bbb) print(aaaa,bb) end", config));

    config.column_limit(10);
    REQUIRE("function W()\n q(aa, bbb)\n print(aaaa, bbb)\nend\n" == lua_format("function W() q(aa,bbb) print(2) end", config));

}

TEST_CASE("table_sep", "config") {
    Config config;
    config.indent_width(2);
    config.table_sep(",");

    REQUIRE("x = {1, 2, 3}\n" == lua_format("x = {1,2;3}", config));

    config.table_sep(";");
    REQUIRE("x = {1; 2; 3}\n" == lua_format("x = {1,2;3}", config));
}

TEST_CASE("extra_sep_at_table_end", "config") {
    Config config;
    config.indent_width(2);
    config.extra_sep_at_table_end(true);

    // do no add sep if table is one line
    REQUIRE("x = {1, 2, 3}\n" == lua_format("x = {1,2;3}", config));

    REQUIRE("x = {\n  1, -- line break\n  2, 3,\n}\n" == lua_format("x = {1,-- line break\n2;3}", config));
    config.extra_sep_at_table_end(false);
    REQUIRE("x = {\n  1, -- line break\n  2, 3\n}\n" == lua_format("x = {1,-- line break\n2;3}", config));
}

TEST_CASE("keep_simple_block_one_line", "config") {
    Config config;
    config.indent_width(2);

    REQUIRE("function x() print(1) end\n" == lua_format("function x() print(1) end", config));

    config.keep_simple_block_one_line(false);
    REQUIRE("function x()\n  print(1)\nend\n" == lua_format("function x() print(1) end", config));
}

TEST_CASE("read from file", "config") {
    Config config;
    config.readFromFile("../test/testconfig/1.lua-format");

    REQUIRE(2 == config.indent_width());
    REQUIRE(";" == config.table_sep());
    REQUIRE(false == config.extra_sep_at_table_end());

    config.readFromFile("../test/testconfig/2.lua-format");

    REQUIRE(4 == config.indent_width());
    REQUIRE("," == config.table_sep());
    REQUIRE(false == config.extra_sep_at_table_end());
}