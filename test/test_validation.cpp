#include <catch2/catch.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "lua-format.h"

using namespace std;
namespace fs = filesystem;

// use for validate_integer
#define TEST_CONFIG_ERROR_FILE(file)                                                                                                                         \
    TEST_CASE("Configuration file " + string(file) + " has an error", "format_file") {                                                                       \
        string configFileName(file);                                                                                                                         \
        Config config;                                                                                                                                       \
        REQUIRE_THROWS_WITH(config.readFromFile(configFileName),"[ERROR] Configuration value is out of range. Must be a positive integer greater than 0.");  \
    }

// use for validate_integer_zero
#define TEST_ZERO_CONFIG_ERROR_FILE(file)                                                                                                                    \
    TEST_CASE("Configuration file " + string(file) + " has an error", "format_file") {                                                                       \
        string configFileName(file);                                                                                                                         \
        Config config;                                                                                                                                       \
        REQUIRE_THROWS_WITH(config.readFromFile(configFileName),"[ERROR] Configuration value is out of range. Must be a positive integer.");                 \
    }

// use for expecting no error
#define TEST_CONFIG_NO_ERROR_FILE(file)                                                                                                                 \
    TEST_CASE("Configuration file " + string(file) + " has no error", "format_file") {                                                                  \
        string configFileName(file);                                                                                                                    \
        Config config;                                                                                                                                  \
        REQUIRE_NOTHROW(config.readFromFile(configFileName));                                                                                           \
    }

TEST_CONFIG_ERROR_FILE(PROJECT_PATH "/test/testdata/Config/column_limit.config");
TEST_CONFIG_ERROR_FILE(PROJECT_PATH "/test/testdata/Config/column_table_limit.config");

TEST_ZERO_CONFIG_ERROR_FILE(PROJECT_PATH "/test/testdata/Config/indent_width.config");

TEST_CONFIG_NO_ERROR_FILE(PROJECT_PATH "/test/testdata/Config/spaces_before_call.config");

