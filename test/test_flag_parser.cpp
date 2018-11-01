#include "FlagParser.h"
#include "catch2/catch.hpp"

TEST_CASE("flag parser works", "flag_parser") {
    const char* argv[]{"lua-format", "-c", "c", "-v", "w"};
    int argc = sizeof(argv) / sizeof(char*);
    FlagParser p;
    p.define("-c", 1);
    p.define("-v", 0);
    p.parse(argc, argv);

    REQUIRE(true == p.hasOption("-c"));
    REQUIRE(true == p.hasOption("-v"));
    REQUIRE(false == p.hasOption("-q"));
    REQUIRE(false == p.hasOption("--config"));

    REQUIRE("c" == p.getOption("-c"));

    REQUIRE(1 == p.getValues().size());
    REQUIRE("w" == p.getValues().front());
}

TEST_CASE("define count", "flag_parser") {
    const char* argv[]{"lua-format", "-c1", "v1", "-c2", "v2", "v3", "v4"};
    int argc = sizeof(argv) / sizeof(char*);
    FlagParser p;
    p.define("-c1", 1);
    p.define("-c2", 2);
    p.parse(argc, argv);

    REQUIRE(true == p.hasOption("-c1"));
    REQUIRE(true == p.hasOption("-c2"));

    REQUIRE(1 == p.getOptions("-c1").size());
    REQUIRE(2 == p.getOptions("-c2").size());
    REQUIRE("v1" == p.getOption("-c1"));
    REQUIRE("v2" == p.getOption("-c2"));
    REQUIRE("v3" == p.getOptions("-c2")[1]);

    REQUIRE(1 == p.getValues().size());
    REQUIRE("v4" == p.getValues().front());
}

TEST_CASE("error count define", "flag_parser") {
    const char* argv[]{"lua-format", "-c1", "v1", "-c2", "v2", "v3", "v4"};
    int argc = sizeof(argv) / sizeof(char*);
    FlagParser p;
    p.define("-c1", 2);
    p.define("-c2", 2);
    string err;
    try {
        p.parse(argc, argv);
    } catch (const invalid_argument& e) {
        err = e.what();
    }

    REQUIRE("-c1: Not enough arguements." == err);
}