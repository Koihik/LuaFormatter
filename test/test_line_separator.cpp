#include <catch2/catch.hpp>

#include "lua-format.h"

TEST_CASE("get_line_separator", "line_separator") {
    REQUIRE(get_line_separator("\n") == "\n");
    REQUIRE(get_line_separator("\r") == "\r");
    REQUIRE(get_line_separator("\r\n") == "\r\n");

    REQUIRE(get_line_separator("1\n2\n3\n") == "\n");
    REQUIRE(get_line_separator("1\r\n2\r\n3\r\n") == "\r\n");
    REQUIRE(get_line_separator("1\r2\r3\r") == "\r");

    // Different separator appears equal times
    REQUIRE(get_line_separator("") == "\n");
    REQUIRE(get_line_separator("\r\n\n\r") == "\n");
    REQUIRE(get_line_separator("\n\r") == "\n");
    REQUIRE(get_line_separator("\r\n\r") == "\r\n");
    REQUIRE(get_line_separator("1\r\n2\n3\r") == "\n");

    // Different separator appears different times
    REQUIRE(get_line_separator("1\r\n2\r\n3\n") == "\r\n");
    REQUIRE(get_line_separator("1\n2\r\n3\r\n") == "\r\n");
    REQUIRE(get_line_separator("1\r2\n3\n") == "\n");
    REQUIRE(get_line_separator("1\n2\r3\r") == "\r");
}

TEST_CASE("convert_line_separator", "line_separator") {
    REQUIRE(convert_line_separator("", "\r\n").empty());
    REQUIRE(convert_line_separator("", "\n").empty());
    REQUIRE(convert_line_separator("", "\r").empty());

    REQUIRE(convert_line_separator("1\r\n2\r\n3\r\n", "\n") == "1\n2\n3\n");
    REQUIRE(convert_line_separator("1\r\n2\r\n3\r\n", "\r") == "1\r2\r3\r");
    REQUIRE(convert_line_separator("1\r\n2\r\n3\r\n", "\r\n") == "1\r\n2\r\n3\r\n");

    REQUIRE(convert_line_separator("1\n2\n3\n", "\n") == "1\n2\n3\n");
    REQUIRE(convert_line_separator("1\n2\n3\n", "\r") == "1\r2\r3\r");
    REQUIRE(convert_line_separator("1\n2\n3\n", "\r\n") == "1\r\n2\r\n3\r\n");

    REQUIRE(convert_line_separator("1\r2\r3\r", "\n") == "1\n2\n3\n");
    REQUIRE(convert_line_separator("1\r2\r3\r", "\r") == "1\r2\r3\r");
    REQUIRE(convert_line_separator("1\r2\r3\r", "\r\n") == "1\r\n2\r\n3\r\n");
}
