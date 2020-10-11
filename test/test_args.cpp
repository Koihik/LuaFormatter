#include <args/args.hxx>
#include <catch2/catch.hpp>
#include <iostream>
#include <tuple>

#include "lua-format.h"

TEST_CASE("indent with args", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=2"});
    config.set("indent_width", args::get(indentwidth));
    REQUIRE("function W()\n  print(1)\n  print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));

    parser.ParseArgs(std::vector<std::string>{"--indent-width=1"});
    config.set("indent_width", args::get(indentwidth));
    REQUIRE("function W()\n print(1)\n print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));
}

TEST_CASE("column_limit with args", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> columnlimit(parser, "column limit", "Column limit of one line", {"column-limit"});
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::ValueFlag<int> continuationindentwidth(parser, "Continuation indentation width",
                                                 "Indent width for continuations line", {"continuation-indent-width"});

    parser.ParseArgs(
        std::vector<std::string>{"--indent-width=1", "--column-limit=10", "--continuation-indent-width=2"});
    config.set("indent_width", args::get(indentwidth));
    config.set("continuation_indent_width", args::get(continuationindentwidth));
    config.set("column_limit", args::get(columnlimit));
    REQUIRE("function W()\n q(aaaa,\n   bbbb)\n print(\n   aaaa,\n   bbbb)\nend\n" ==
            lua_format("function W() q(aaaa,bbbb) print(aaaa,bbbb) end", config));

    parser.ParseArgs(std::vector<std::string>{"--column-limit=80"});
    config.set("column_limit", args::get(columnlimit));
    REQUIRE("function W()\n q(aaaa, bbbb)\n print(aaaa, bbbb)\nend\n" ==
            lua_format("function W() q(aaaa,bbbb) print(aaaa,bbbb) end", config));
}

TEST_CASE("spaces_before_call with args", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> columnlimit(parser, "column limit", "Column limit of one line", {"column-limit"});
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::ValueFlag<int> continuationindentwidth(parser, "Continuation indentation width",
                                                 "Indent width for continuations line", {"continuation-indent-width"});
    args::ValueFlag<int> spacesbeforecall(parser, "spaces before call", "Space on function calls",
                                          {"spaces-before-call"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=4", "--column-limit=40", "--continuation-indent-width=4",
                                              "--spaces-before-call=0"});
    config.set("indent_width", args::get(indentwidth));
    config.set("continuation_indent_width", args::get(continuationindentwidth));
    config.set("column_limit", args::get(columnlimit));
    config.set("spaces_before_call", args::get(spacesbeforecall));
    REQUIRE(
        "function t() return function() end end\nlocal a = t{1, 2, 3, 4}\nlocal a = t{1}\nlocal a = t{8}\nlocal a = "
        "t{1, 9}\nlocal a = t(1, 2, 3)\nt{2, 3} {1, 5}\nt{2, 3, 4} {1}\n" ==
        lua_format("function t() return function () end end local a = t{1, 2, 3, 4} local a = t        {1} local a = "
                   "t{8} local a = t  {1,9} local a = t(1, 2, 3) t {2, 3}{1,5} t{2, 3, 4}{1}",
                   config));
}

TEST_CASE("use_tab with args", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::Group optusetab(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag usetab(optusetab, "Tab indentation", "Use tab for indentation", {"use-tab"});
    args::Flag nousetab(optusetab, "Tab indentation", "Do not use tab for indentation", {"no-use-tab"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=1", "--use-tab"});
    config.set("indent_width", args::get(indentwidth));
    if (usetab) {
        config.set("use_tab", true);
    } else if (nousetab) {
        config.set("use_tab", false);
    }
    REQUIRE("function W()\n\tprint(1)\n\tprint(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));

    parser.ParseArgs(std::vector<std::string>{"--no-use-tab"});
    if (usetab) {
        config.set("use_tab", true);
    } else if (nousetab) {
        config.set("use_tab", false);
    }
    REQUIRE("function W()\n print(1)\n print(2)\nend\n" == lua_format("function W() print(1) print(2) end", config));
}

TEST_CASE("table_sep with args", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::ValueFlag<char> tablesep(parser, "table separator", "Character to separate table fields", {"table-sep"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=2", "--table-sep=,"});
    config.set("indent_width", args::get(indentwidth));
    config.set("table_sep", args::get(tablesep));

    REQUIRE("x = {1, 2, 3}\n" == lua_format("x = {1,2;3}", config));

    parser.ParseArgs(std::vector<std::string>{"--table-sep=;"});
    config.set("table_sep", args::get(tablesep));
    REQUIRE("x = {1; 2; 3}\n" == lua_format("x = {1,2;3}", config));
}

TEST_CASE("extra_sep_at_table_end with args", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::Group opttableEnd(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag tableEnd(opttableEnd, "separator at end", "Add a extra field separator", {"extra-sep-at-table-end"});
    args::Flag notableEnd(opttableEnd, "separator at end", "Do not add a extra field separator",
                          {"no-extra-sep-at-table-end"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=2", "--extra-sep-at-table-end"});
    config.set("indent_width", args::get(indentwidth));
    if (tableEnd) {
        config.set("extra_sep_at_table_end", true);
    } else if (notableEnd) {
        config.set("extra_sep_at_table_end", false);
    }

    // do no add sep if table is one line
    REQUIRE("x = {1, 2, 3}\n" == lua_format("x = {1,2;3}", config));

    REQUIRE("x = {\n  1, -- line break\n  2, 3,\n}\n" == lua_format("x = {1,-- line break\n2;3}", config));

    parser.ParseArgs(std::vector<std::string>{"--no-extra-sep-at-table-end"});
    if (tableEnd) {
        config.set("extra_sep_at_table_end", true);
    } else if (notableEnd) {
        config.set("extra_sep_at_table_end", false);
    }
    REQUIRE("x = {\n  1, -- line break\n  2, 3\n}\n" == lua_format("x = {1,-- line break\n2;3}", config));
}

TEST_CASE("keep_simple_function_one_line with args", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::Group optfunctionline(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag functionline(optfunctionline, "function line", "keep function in one line",
                            {"keep-simple-function-one-line"});
    args::Flag nofunctionline(optfunctionline, "function line", "Do not keep function in one line",
                              {"no-keep-simple-function-one-line"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=2"});
    config.set("indent_width", args::get(indentwidth));
    REQUIRE("function x() print(1) end\n" == lua_format("function x() print(1) end", config));

    parser.ParseArgs(std::vector<std::string>{"--no-keep-simple-function-one-line"});
    if (functionline) {
        config.set("keep_simple_function_one_line", true);
    } else if (nofunctionline) {
        config.set("keep_simple_function_one_line", false);
    }
    REQUIRE("function x()\n  print(1)\nend\n" == lua_format("function x() print(1) end", config));
}

TEST_CASE("args with config", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::ValueFlag<int> continuationindentwidth(parser, "Continuation indentation width",
                                                 "Indent width for continuations line", {"continuation-indent-width"});
    args::ValueFlag<int> columnlimit(parser, "column limit", "Column limit of one line", {"column-limit"});
    args::Group optalignargs(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag alignargs(optalignargs, "align args", "Align the arguments", {"align-args"});
    args::Flag noalignargs(optalignargs, "align args", "Do not align the arguments", {"no-align-args"});
    args::Group optfunctioncallLP(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag functioncallLP(optfunctioncallLP, "functioncall lp", "Break after '(' of function call",
                              {"break-after-functioncall-lp"});
    args::Flag nofunctioncallLP(optfunctioncallLP, "functioncall lp", "Do not break after '(' of function call",
                                {"no-break-after-functioncall-lp"});
    args::Group optfunctioncallRP(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag functioncallRP(optfunctioncallRP, "functioncall rp", "Break before ')' of function call",
                              {"break-before-functioncall-rp"});
    args::Flag nofunctioncallRP(optfunctioncallRP, "functioncall rp", "Do not break before ')' of function call",
                                {"no-break-before-functioncall-rp"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=2", "--continuation-indent-width=4", "--column-limit=10",
                                              "--align-args", "--no-break-after-functioncall-lp",
                                              "--no-break-before-functioncall-rp"});
    config.set("indent_width", args::get(indentwidth));
    config.set("continuation_indent_width", args::get(continuationindentwidth));
    if (alignargs) {
        config.set("align_args", true);
    } else if (noalignargs) {
        config.set("align_args", false);
    }
    if (functioncallLP) {
        config.set("break_after_functioncall_lp", true);
    } else if (nofunctioncallLP) {
        config.set("break_after_functioncall_lp", false);
    }
    if (functioncallRP) {
        config.set("break_before_functioncall_rp", true);
    } else if (nofunctioncallRP) {
        config.set("break_before_functioncall_rp", false);
    }
    config.set("column_limit", args::get(columnlimit));
    REQUIRE("xxx(xxx,\n    xxxxx)\n" == lua_format("xxx(xxx,xxxxx)", config));

    parser.ParseArgs(std::vector<std::string>{"--break-after-functioncall-lp"});
    if (functioncallLP) {
        config.set("break_after_functioncall_lp", true);
    } else if (nofunctioncallLP) {
        config.set("break_after_functioncall_lp", false);
    }
    REQUIRE("xxx(\n    xxx,\n    xxxxx)\n" == lua_format("xxx(xxx,xxxxx)", config));
    REQUIRE("xxx(\n    xxxxxxxxxx,\n    x)\n" == lua_format("xxx(xxxxxxxxxx,x)", config));

    parser.ParseArgs(std::vector<std::string>{"--break-before-functioncall-rp"});
    if (functioncallRP) {
        config.set("break_before_functioncall_rp", true);
    } else if (nofunctioncallRP) {
        config.set("break_before_functioncall_rp", false);
    }
    REQUIRE("xxx(\n    xxx,\n    xxxxx\n)\n" == lua_format("xxx(xxx,xxxxx)", config));
    REQUIRE("xxx(\n    xxxxx,\n    xxx\n)\n" == lua_format("xxx(xxxxx,xxx)", config));

    parser.ParseArgs(std::vector<std::string>{"--no-align-args", "--no-break-after-functioncall-lp",
                                              "--no-break-before-functioncall-rp"});
    if (alignargs) {
        config.set("align_args", true);
    } else if (noalignargs) {
        config.set("align_args", false);
    }
    if (functioncallLP) {
        config.set("break_after_functioncall_lp", true);
    } else if (nofunctioncallLP) {
        config.set("break_after_functioncall_lp", false);
    }
    if (functioncallRP) {
        config.set("break_before_functioncall_rp", true);
    } else if (nofunctioncallRP) {
        config.set("break_before_functioncall_rp", false);
    }
    REQUIRE("x(xxxxxx,\n    xxxxx)\n" == lua_format("x(xxxxxx,xxxxx)", config));
}

TEST_CASE("functiondef with args", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::ValueFlag<int> continuationindentwidth(parser, "Continuation indentation width",
                                                 "Indent width for continuations line", {"continuation-indent-width"});
    args::ValueFlag<int> columnlimit(parser, "column limit", "Column limit of one line", {"column-limit"});
    args::Group optalignparameter(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag alignparameter(optalignparameter, "align parameter", "Align the parameters", {"align-parameter"});
    args::Flag noalignparameter(optalignparameter, "align parameter", "Do not align the parameters",
                                {"no-align-parameter"});
    args::Group optfunctiondefLP(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag functiondefLP(optfunctiondefLP, "functiondef lp", "Break after '(' of function def",
                             {"break-after-functiondef-lp"});
    args::Flag nofunctiondefLP(optfunctiondefLP, "functiondef lp", "Do not break after '(' of function def",
                               {"no-break-after-functiondef-lp"});
    args::Group optfunctiondefRP(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag functiondefRP(optfunctiondefRP, "functiondef rp", "Break before ')' of function def",
                             {"break-before-functiondef-rp"});
    args::Flag nofunctiondefRP(optfunctiondefRP, "functiondef rp", "Do not break before ')' of function def",
                               {"no-break-before-functiondef-rp"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=2", "--continuation-indent-width=4", "--column-limit=20",
                                              "--align-parameter", "--no-break-after-functiondef-lp",
                                              "--no-break-before-functiondef-rp"});
    config.set("indent_width", args::get(indentwidth));
    config.set("continuation_indent_width", args::get(continuationindentwidth));
    if (alignparameter) {
        config.set("align_parameter", true);
    } else if (noalignparameter) {
        config.set("align_parameter", false);
    }
    if (functiondefLP) {
        config.set("break_after_functiondef_lp", true);
    } else if (nofunctiondefLP) {
        config.set("break_after_functiondef_lp", false);
    }
    if (functiondefRP) {
        config.set("break_before_functiondef_rp", true);
    } else if (nofunctiondefRP) {
        config.set("break_before_functiondef_rp", false);
    }
    config.set("column_limit", args::get(columnlimit));
    REQUIRE("function xxx(xxx,\n             xxxxx)\n  print(1)\nend\n" ==
            lua_format("function xxx(xxx,xxxxx)\n  print(1)\nend\n", config));

    parser.ParseArgs(std::vector<std::string>{"--break-after-functiondef-lp"});
    if (functiondefLP) {
        config.set("break_after_functiondef_lp", true);
    } else if (nofunctiondefLP) {
        config.set("break_after_functiondef_lp", false);
    }
    REQUIRE("function xxx(\n    xxx, xxxxx)\n  print(1)\nend\n" ==
            lua_format("function xxx(xxx,xxxxx)\n  print(1)\nend\n", config));
    REQUIRE("function a(x) end\n" == lua_format("function a(x) end", config));

    parser.ParseArgs(std::vector<std::string>{"--break-before-functiondef-rp"});
    if (functiondefRP) {
        config.set("break_before_functiondef_rp", true);
    } else if (nofunctiondefRP) {
        config.set("break_before_functiondef_rp", false);
    }
    REQUIRE("function xxx(\n    xxx, xxxxx\n) print(1) end\n" ==
            lua_format("function xxx(xxx,xxxxx)\n  print(1)\nend\n", config));
    REQUIRE("function a(x) end\n" == lua_format("function a(x) end", config));

    parser.ParseArgs(std::vector<std::string>{"--no-align-parameter", "--no-break-after-functiondef-lp",
                                              "--no-break-before-functiondef-rp"});
    if (alignparameter) {
        config.set("align_parameter", true);
    } else if (noalignparameter) {
        config.set("align_parameter", false);
    }
    if (functiondefLP) {
        config.set("break_after_functiondef_lp", true);
    } else if (nofunctiondefLP) {
        config.set("break_after_functiondef_lp", false);
    }
    if (functiondefRP) {
        config.set("break_before_functiondef_rp", true);
    } else if (nofunctiondefRP) {
        config.set("break_before_functiondef_rp", false);
    }
    REQUIRE("function xxx(xxx,\n    xxxxx) print(1) end\n" ==
            lua_format("function xxx(xxx,xxxxx)\n  print(1)\nend\n", config));

    args::Group optchopdownparameter(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag chopdownparameter(optchopdownparameter, "chop down parameter", "Chop down all parameters",
                                 {"chop-down-parameter"});
    args::Flag nochopdownparameter(optchopdownparameter, "chop down parameter", "Do not chop down all parameters",
                                   {"no-chop-down-parameter"});

    parser.ParseArgs(std::vector<std::string>{"--chop-down-parameter"});
    if (chopdownparameter) {
        config.set("chop_down_parameter", true);
    } else if (nochopdownparameter) {
        config.set("chop_down_parameter", false);
    }
    REQUIRE("function xxx(xxx,\n    yyy,\n    zzz) print(1) end\n" ==
            lua_format("function xxx(xxx,yyy,zzz)\n  print(1)\nend\n", config));
}

TEST_CASE("table with args", "[args]") {
    Config config;
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::ValueFlag<int> continuationindentwidth(parser, "Continuation indentation width",
                                                 "Indent width for continuations line", {"continuation-indent-width"});
    args::ValueFlag<int> columnlimit(parser, "column limit", "Column limit of one line", {"column-limit"});
    args::Group optaligntablefield(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag aligntablefield(optaligntablefield, "align table field", "Align fields of table", {"align-table-field"});
    args::Flag noaligntablefield(optaligntablefield, "align table field", "Do not align fields of table",
                                 {"no-align-table-field"});
    args::Group opttableLB(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag tableLB(opttableLB, "table LB", "Break after '{' of table", {"break-after-table-lb"});
    args::Flag notableLB(opttableLB, "table LB", "Do not break after '{' of table", {"no-break-after-table-lb"});
    args::Group opttableRB(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag tableRB(opttableRB, "table RB", "Break before '}' of table", {"break-before-table-rb"});
    args::Flag notableRB(opttableRB, "table RB", "Do not break before '}' of table", {"no-break-before-table-rb"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=2", "--continuation-indent-width=4", "--column-limit=15",
                                              "--align-table-field", "--no-break-after-table-lb",
                                              "--no-break-before-table-rb"});
    config.set("indent_width", args::get(indentwidth));
    config.set("continuation_indent_width", args::get(continuationindentwidth));
    if (aligntablefield) {
        config.set("align_table_field", true);
    } else if (noaligntablefield) {
        config.set("align_table_field", false);
    }
    if (tableLB) {
        config.set("break_after_table_lb", true);
    } else if (notableLB) {
        config.set("break_after_table_lb", false);
    }
    if (tableRB) {
        config.set("break_before_table_rb", true);
    } else if (notableRB) {
        config.set("break_before_table_rb", false);
    }
    config.set("column_limit", args::get(columnlimit));

    REQUIRE("x = {1, 2, 3,\n     4, 5, 6, 7}\n" == lua_format("x = {1,2,3,4,5,6,7}", config));

    parser.ParseArgs(std::vector<std::string>{"--break-after-table-lb"});
    if (tableLB) {
        config.set("break_after_table_lb", true);
    } else if (notableLB) {
        config.set("break_after_table_lb", false);
    }
    REQUIRE("x = {\n  1, 2, 3, 4,\n  5, 6, 7}\n" == lua_format("x = {1,2,3,4,5,6,7}", config));
    REQUIRE("x = {1, 2}\n" == lua_format("x = {1,2}", config));

    parser.ParseArgs(std::vector<std::string>{"--break-before-table-rb"});
    if (tableRB) {
        config.set("break_before_table_rb", true);
    } else if (notableRB) {
        config.set("break_before_table_rb", false);
    }
    REQUIRE("x = {\n  1, 2, 3, 4,\n  5, 6, 7\n}\n" == lua_format("x = {1,2,3,4,5,6,7}", config));
    REQUIRE("x = {1, 2}\n" == lua_format("x = {1,2}", config));

    parser.ParseArgs(
        std::vector<std::string>{"--no-align-table-field", "--no-break-after-table-lb", "--no-break-before-table-rb"});
    if (aligntablefield) {
        config.set("align_table_field", true);
    } else if (noaligntablefield) {
        config.set("align_table_field", false);
    }
    if (tableLB) {
        config.set("break_after_table_lb", true);
    } else if (notableLB) {
        config.set("break_after_table_lb", false);
    }
    if (tableRB) {
        config.set("break_before_table_rb", true);
    } else if (notableRB) {
        config.set("break_before_table_rb", false);
    }
    REQUIRE("x = {1, 2, 3,\n  4, 5, 6, 7}\n" == lua_format("x = {1,2,3,4,5,6,7}", config));

    args::ValueFlag<int> tabwidth(parser, "tab width", "Number of spaces used per tab", {"tab-width"});
    args::Group optchopdowntable(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag chopdowntable(optchopdowntable, "chop down table", "Chop down any table", {"chop-down-table"});
    args::Flag nochopdowntable(optchopdowntable, "chop down table", "Do not chop down any table",
                               {"no-chop-down-table"});
    args::Group optchopdownkvtable(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
    args::Flag chopdownkvtable(optchopdownkvtable, "chop down table keys", "Chop down table if table contains key",
                               {"chop-down-kv-table"});
    args::Flag nochopdownkvtable(optchopdownkvtable, "chop down table keys",
                                 "Do not chop down table if table contains key", {"no-chop-down-kv-table"});
    args::ValueFlag<int> columntablelimit(parser, "column table limit", "Column limit of each line of a table",
                                          {"column-table-limit"});

    parser.ParseArgs(std::vector<std::string>{"--indent-width=2", "--continuation-indent-width=2", "--column-limit=250",
                                              "--chop-down-table", "--chop-down-kv-table", "--tab-width=2",
                                              "--column-table-limit=80"});
    config.set("column_limit", args::get(columnlimit));
    config.set("indent_width", args::get(indentwidth));
    config.set("tab_width", args::get(tabwidth));
    config.set("continuation_indent_width", args::get(continuationindentwidth));
    if (chopdowntable) {
        config.set("chop_down_table", true);
    } else if (nochopdowntable) {
        config.set("chop_down_table", false);
    }
    if (chopdownkvtable) {
        config.set("chop_down_kv_table", true);
    } else if (nochopdownkvtable) {
        config.set("chop_down_kv_table", false);
    }
    config.set("column_table_limit", args::get(columntablelimit));

    REQUIRE(
        "test = {\n  image = {1, 2, 3},\n  list = {\n    {\n      ref = {4, 5, 9, 8, 2},\n      tags = {1, 2, 8, 6},\n "
        "     time = 10,\n\n      materials = {{materialId = 123, count = 10}}\n    }\n  }\n}\n" ==
        lua_format("test = {\n  image = {1,2,3},\n  list = {\n    {\n      ref = {4,5,9,8,2},\n      tags = { 1, 2, 8, "
                   "6 },\n      time = 10,\n\n      materials = {\n        {\n          materialId = 123,\n          "
                   "count = 10\n        }\n      },\n    },\n  },\n}",
                   config));
}
