#include "catch2/catch.hpp"
#include "lua-format.h"

TEST_CASE("indent", "config") {
    Config config;
    config.indent_width(2);
    REQUIRE("function W()\n  print(1)\n  print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));

    config.indent_width(1);
    REQUIRE("function W()\n print(1)\n print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));
}

TEST_CASE("use_tab", "config") {
    Config config;
    config.indent_width(1);
    config.use_tab(true);

    REQUIRE("function W()\n\tprint(1)\n\tprint(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));

    config.use_tab(false);
    REQUIRE("function W()\n print(1)\n print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));
}

TEST_CASE("column_limit", "config") {
    Config config;
    config.indent_width(1);
    config.continuation_indent_width(2);
    config.column_limit(10);
    REQUIRE("function W()\n q(aaaa,\n   bbbb)\n print(\n   aaaa,\n   bbbb)\nend\n" == lua_format("function W() q(aaaa,bbbb) print(aaaa,bbbb) end", config));

    config.column_limit(80);
    REQUIRE("function W()\n q(aaaa, bbbb)\n print(aaaa, bbbb)\nend\n" == lua_format("function W() q(aaaa,bbbb) print(aaaa,bbbb) end", config));
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

TEST_CASE("args", "config") {
    Config config;
    config.indent_width(2);
    config.continuation_indent_width(4);
    config.align_args(true);
    config.break_after_functioncall_lp(false);
    config.break_before_functioncall_rp(false);
    config.column_limit(10);

    REQUIRE("xxx(xxx,\n    xxxxx)\n" == lua_format("xxx(xxx,xxxxx)", config));

    config.break_after_functioncall_lp(true);
    REQUIRE("xxx(\n    xxx,\n    xxxxx)\n" == lua_format("xxx(xxx,xxxxx)", config));
    REQUIRE("x(x, x)\n" == lua_format("x(x,x)", config));

    config.break_before_functioncall_rp(true);
    REQUIRE("xxx(\n    xxx,\n    xxxxx\n)\n" == lua_format("xxx(xxx,xxxxx)", config));
    REQUIRE("x(x, x)\n" == lua_format("x(x,x)", config));

    config.align_args(false);
    config.break_after_functioncall_lp(false);
    config.break_before_functioncall_rp(false);
    REQUIRE("x(xxxxxx,\n    xxxxx)\n" == lua_format("x(xxxxxx,xxxxx)", config));
}

TEST_CASE("functiondef", "config") {
    Config config;
    config.indent_width(2);
    config.continuation_indent_width(4);
    config.align_parameter(true);
    config.break_after_functiondef_lp(false);
    config.break_before_functiondef_rp(false);
    config.column_limit(20);

    REQUIRE("function xxx(xxx,\n             xxxxx)\n  print(1)\nend\n" == lua_format("function xxx(xxx,xxxxx)\n  print(1)\nend\n", config));

    config.break_after_functiondef_lp(true);
    REQUIRE("function xxx(\n    xxx, xxxxx)\n  print(1)\nend\n" == lua_format("function xxx(xxx,xxxxx)\n  print(1)\nend\n", config));
    REQUIRE("function a(x) end\n" == lua_format("function a(x) end", config));

    config.break_before_functiondef_rp(true);
    REQUIRE("function xxx(\n    xxx, xxxxx\n) print(1) end\n" == lua_format("function xxx(xxx,xxxxx)\n  print(1)\nend\n", config));
    REQUIRE("function a(x) end\n" == lua_format("function a(x) end", config));

    config.align_parameter(false);
    config.break_after_functiondef_lp(false);
    config.break_before_functiondef_rp(false);
    REQUIRE("function xxx(xxx,\n    xxxxx) print(1) end\n" == lua_format("function xxx(xxx,xxxxx)\n  print(1)\nend\n", config));

    config.chop_down_parameter(true);
    REQUIRE("function xxx(xxx,\n    yyy,\n    zzz) print(1) end\n" == lua_format("function xxx(xxx,yyy,zzz)\n  print(1)\nend\n", config));
}

TEST_CASE("table", "config") {
    Config config;
    config.indent_width(2);
    config.continuation_indent_width(4);
    config.align_table_field(true);
    config.break_after_table_lb(false);
    config.break_before_table_rb(false);
    config.column_limit(15);

    REQUIRE("x = {1, 2, 3,\n     4, 5, 6, 7}\n" == lua_format("x = {1,2,3,4,5,6,7}", config));

    config.break_after_table_lb(true);
    REQUIRE("x = {\n  1, 2, 3, 4,\n  5, 6, 7}\n" == lua_format("x = {1,2,3,4,5,6,7}", config));
    REQUIRE("x = {1, 2}\n" == lua_format("x = {1,2}", config));

    config.break_before_table_rb(true);
    REQUIRE("x = {\n  1, 2, 3, 4,\n  5, 6, 7\n}\n" == lua_format("x = {1,2,3,4,5,6,7}", config));
    REQUIRE("x = {1, 2}\n" == lua_format("x = {1,2}", config));

    config.align_table_field(false);
    config.break_after_table_lb(false);
    config.break_before_table_rb(false);
    REQUIRE("x = {1, 2, 3,\n  4, 5, 6, 7}\n" == lua_format("x = {1,2,3,4,5,6,7}", config));
}

TEST_CASE("break_after_operator", "config") {
    Config config;
    config.indent_width(2);
    config.continuation_indent_width(4);
    config.column_limit(10);
    config.break_after_operator(true);

    REQUIRE("x = 111 +\n        1111111 +\n        1111111\n" == lua_format("x = 111 + 1111111 + 1111111", config));

    config.break_after_operator(false);
    REQUIRE("x = 111\n        + 1111111\n        + 1111111\n" == lua_format("x = 111 + 1111111 + 1111111", config));

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