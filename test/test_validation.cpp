#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "lua-format.h"

TEST_CASE("gt0 validator", "[config_validator]") {
    std::string configFileName(PROJECT_PATH "/test/config/column_limit.config");
    Config config;
    REQUIRE_THROWS_WITH(config.readFromFile(configFileName),
                        "[ERROR] Configuration value is out of range. Must be a positive integer greater than 0.");
}

TEST_CASE("ge0 validator", "[config_validator]") {
    std::string configFileName(PROJECT_PATH "/test/config/indent_width.config");
    Config config;
    REQUIRE_THROWS_WITH(config.readFromFile(configFileName),
                        "[ERROR] Configuration value is out of range. Must be a positive integer.");

    std::string configFileName2(PROJECT_PATH "/test/config/column_table_limit.config");
    Config config2;
    REQUIRE_THROWS_WITH(config2.readFromFile(configFileName2),
                        "[ERROR] Configuration value is out of range. Must be a positive integer.");
}

TEST_CASE("correct config file", "[config_validator]") {
    std::string configFileName(PROJECT_PATH "/test/config/correct.config");
    Config config;
    REQUIRE_NOTHROW(config.readFromFile(configFileName));
}

TEST_CASE("quote validator", "[config_validator]") {
    std::string configFileName(PROJECT_PATH "/test/config/single_to_double.config");
    Config config;
    REQUIRE_THROWS_WITH(config.readFromFile(configFileName),
                        "[ERROR] Configuration value of single_quote_to_double_quote is conflicting with the value of double_quote_to_single_quote");
}

TEST_CASE("tab validator", "[config_validator]") {
    std::string configFileName(PROJECT_PATH "/test/config/tab.config");
    Config config;
    REQUIRE_THROWS_WITH(config.readFromFile(configFileName),
                        "[ERROR] Configuration value of use_tab is conflicting with the value of tab_width");
}
