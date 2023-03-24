#include "Config.h"

#include <any>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>

Config::Config() {
    // Default configuration
    node["column_limit"] = 80;
    node["indent_width"] = 4;
    node["use_tab"] = false;
    node["tab_width"] = 4;
    node["continuation_indent_width"] = 4;
    node["spaces_before_call"] = 1;

    node["keep_simple_control_block_one_line"] = true;
    node["keep_simple_function_one_line"] = true;

    node["align_args"] = true;
    node["break_after_functioncall_lp"] = false;
    node["break_before_functioncall_rp"] = false;
    node["spaces_inside_functioncall_parens"] = false;
    node["spaces_inside_functiondef_parens"] = false;

    node["align_parameter"] = true;
    node["chop_down_parameter"] = false;
    node["break_after_functiondef_lp"] = false;
    node["break_before_functiondef_rp"] = false;

    node["align_table_field"] = true;
    node["break_after_table_lb"] = true;
    node["break_before_table_rb"] = true;
    node["chop_down_table"] = false;
    node["chop_down_kv_table"] = true;
    node["table_sep"] = ",";
    node["extra_sep_at_table_end"] = false;
    node["column_table_limit"] = 0; // follows column_limit by default
    node["column_table_limit_kv"] = 0;  // follows column_table_limit by default
    node["spaces_inside_table_braces"] = false;

    node["break_after_operator"] = true;

    node["double_quote_to_single_quote"] = false;
    node["single_quote_to_double_quote"] = false;

    node["spaces_around_equals_in_field"] = true;
    node["line_breaks_after_function_body"] = 1;

    node["line_separator"] = "input";

    // Validators
    // validate integer without 0s as configuration value
    auto validate_integer = [](const std::string& key, std::any elem) {
        int value = std::any_cast<int>(elem);
        if (value > 0) {
            return value;
        }
        std::cerr << "[ERROR] Processing " << key << " failed" << std::endl;
        throw std::domain_error(
            "[ERROR] Configuration value is out of range. Must be a positive integer greater than 0.");
    };

    // validate integer with 0s as configuration value
    auto validate_integer_zero = [](const std::string& key, std::any elem) {
        int value = std::any_cast<int>(elem);
        if (value >= 0) {
            return value;
        }
        std::cerr << "[ERROR] Processing " << key << " failed" << std::endl;
        throw std::domain_error("[ERROR] Configuration value is out of range. Must be a positive integer.");
    };
    auto validate_boolean = [](const std::string& key, std::any elem) {
        (void)(key);
        bool value = std::any_cast<bool>(elem);
        return value;
    };
    auto validate_character = [](const std::string& key, std::any elem) {
        (void)(key);
        char value = std::any_cast<char>(elem);
        return value;
    };
    auto validate_quote = [&](const std::string& key, std::any elem) {
        bool value = std::any_cast<bool>(elem);
        if (key == "double_quote_to_single_quote") {
            if (value && (this->get<bool>("single_quote_to_double_quote"))) {
                throw std::logic_error(
                    "[ERROR] Configuration value of double_quote_to_single_quote is conflicting with the value of "
                    "single_quote_to_double_quote");
            }
        } else if (key == "single_quote_to_double_quote") {
            if (value && (this->get<bool>("double_quote_to_single_quote"))) {
                throw std::logic_error(
                    "[ERROR] Configuration value of single_quote_to_double_quote is conflicting with the value of "
                    "double_quote_to_single_quote");
            }
        }
        return value;
    };
    auto validate_use_tab = [&](const std::string& key, std::any elem) {
        (void)(key);
        bool value = std::any_cast<bool>(elem);
        if (value && (this->get<int>("tab_width") == 0)) {
            throw std::logic_error("[ERROR] Configuration value of use_tab is conflicting with the value of tab_width");
        }
        return value;
    };
    auto validate_tab_width = [&](const std::string& key, std::any elem) {
        (void)(key);
        int value = std::any_cast<int>(elem);
        if (value < 0) {
            throw std::domain_error(
                "[ERROR] Configuration value of tab_width is out of range. Must be a positive integer.");
        }
        if (value == 0 && (this->get<bool>("use_tab"))) {
            throw std::logic_error("[ERROR] Configuration value of tab_width is conflicting with the value of use_tab");
        }
        return value;
    };
    auto validate_line_separator = [&](const std::string& key, std::any elem) {
        (void)(key);
        const auto value = std::any_cast<std::string>(elem);
        if (value != "os" && value != "input" && value != "lf" && value != "cr" && value != "crlf") {
            throw std::domain_error(
                "[ERROR] Configuration value of line_separator should be one of os/input/lf/cr/crlf");
        }
        return value;
    };

    // Validators
    validators["spaces_before_call"] = validate_integer_zero;
    validators["column_limit"] = validate_integer;
    validators["indent_width"] = validate_integer_zero;
    validators["tab_width"] = validate_tab_width;
    validators["continuation_indent_width"] = validate_integer_zero;
    validators["column_table_limit"] = validate_integer_zero;
    validators["column_table_limit_kv"] = validate_integer_zero;
    validators["use_tab"] = validate_use_tab;
    validators["keep_simple_control_block_one_line"] = validate_boolean;
    validators["keep_simple_function_one_line"] = validate_boolean;
    validators["align_args"] = validate_boolean;
    validators["break_after_functioncall_lp"] = validate_boolean;
    validators["break_before_functioncall_rp"] = validate_boolean;
    validators["align_parameter"] = validate_boolean;
    validators["chop_down_parameter"] = validate_boolean;
    validators["break_after_functiondef_lp"] = validate_boolean;
    validators["break_before_functiondef_rp"] = validate_boolean;
    validators["align_table_field"] = validate_boolean;
    validators["break_after_table_lb"] = validate_boolean;
    validators["break_before_table_rb"] = validate_boolean;
    validators["chop_down_table"] = validate_boolean;
    validators["chop_down_kv_table"] = validate_boolean;
    validators["extra_sep_at_table_end"] = validate_boolean;
    validators["break_after_operator"] = validate_boolean;
    validators["double_quote_to_single_quote"] = validate_quote;
    validators["single_quote_to_double_quote"] = validate_quote;
    validators["table_sep"] = validate_character;
    validators["spaces_inside_functioncall_parens"] = validate_boolean;
    validators["spaces_inside_functiondef_parens"] = validate_boolean;
    validators["spaces_inside_table_braces"] = validate_boolean;
    validators["spaces_around_equals_in_field"] = validate_boolean;
    validators["line_breaks_after_function_body"] = validate_integer;
    validators["line_separator"] = validate_line_separator;

    // DataType of every configuration field
    datatype["spaces_before_call"] = 'i';
    datatype["column_limit"] = 'i';
    datatype["indent_width"] = 'i';
    datatype["tab_width"] = 'i';
    datatype["continuation_indent_width"] = 'i';
    datatype["column_table_limit"] = 'i';
    datatype["column_table_limit_kv"] = 'i';
    datatype["use_tab"] = 'b';
    datatype["keep_simple_control_block_one_line"] = 'b';
    datatype["keep_simple_function_one_line"] = 'b';
    datatype["align_args"] = 'b';
    datatype["break_after_functioncall_lp"] = 'b';
    datatype["break_before_functioncall_rp"] = 'b';
    datatype["align_parameter"] = 'b';
    datatype["chop_down_parameter"] = 'b';
    datatype["break_after_functiondef_lp"] = 'b';
    datatype["break_before_functiondef_rp"] = 'b';
    datatype["align_table_field"] = 'b';
    datatype["break_after_table_lb"] = 'b';
    datatype["break_before_table_rb"] = 'b';
    datatype["chop_down_table"] = 'b';
    datatype["chop_down_kv_table"] = 'b';
    datatype["extra_sep_at_table_end"] = 'b';
    datatype["break_after_operator"] = 'b';
    datatype["double_quote_to_single_quote"] = 'b';
    datatype["single_quote_to_double_quote"] = 'b';
    datatype["table_sep"] = 'c';
    datatype["spaces_inside_functioncall_parens"] = 'b';
    datatype["spaces_inside_functiondef_parens"] = 'b';
    datatype["spaces_inside_table_braces"] = 'b';
    datatype["spaces_around_equals_in_field"] = 'b';
    datatype["line_breaks_after_function_body"] = 'i';
    datatype["line_separator"] = 's';
}

void Config::readFromFile(const std::string& file) {
    fs::file_status status = fs::status(file);
    fs::perms perm = status.permissions();

    if (!fs::is_regular_file(status)) {
        std::cerr << file << ": Not a file." << std::endl;
        exit(-1);
    }

    if ((perm & fs::perms::owner_read) == fs::perms::none) {
        std::cerr << file << ": No access to read." << std::endl;
        exit(-1);
    }

    YAML::Node n = YAML::LoadFile(file);

    // Keys are always strings
    for (const auto& kv : n) {
        auto key = kv.first.as<std::string>();
        if (node[key]) {
            switch (datatype[key]) {
                case 'i': {
                    auto value = kv.second.as<int>();
                    node[key] = std::any_cast<int>(validators[key](key, value));
                    break;
                }
                case 'b': {
                    auto value = kv.second.as<bool>();
                    node[key] = std::any_cast<bool>(validators[key](key, value));
                    break;
                }
                case 'c': {
                    auto value = kv.second.as<char>();
                    node[key] = std::any_cast<char>(validators[key](key, value));
                    break;
                }
                case 's': {
                    auto value = kv.second.as<std::string>();
                    node[key] = std::any_cast<std::string>(validators[key](key, value));
                    break;
                }
            }
        }
    }
}

void Config::readFromMap(std::map<std::string, std::any>& mp) {
    for (const auto& kv : mp) {
        auto key = kv.first;
        if (node[key]) {
            switch (datatype[key]) {
                case 'i': {
                    assert(strcmp(kv.second.type().name(), "i") == 0);
                    node[key] = std::any_cast<int>(validators[key](key, kv.second));
                    break;
                }
                case 'b': {
                    assert(strcmp(kv.second.type().name(), "b") == 0);
                    node[key] = std::any_cast<bool>(validators[key](key, kv.second));
                    break;
                }
                case 'c': {
                    assert(strcmp(kv.second.type().name(), "c") == 0);
                    node[key] = std::any_cast<char>(validators[key](key, kv.second));
                    break;
                }
                case 's': {
                    assert(strcmp(kv.second.type().name(), "s") == 0);
                    node[key] = std::any_cast<std::string>(validators[key](key, kv.second));
                    break;
                }
            }
        }
    }
}

void Config::dumpCurrent(std::ofstream& fout) {
    for (const auto& kv : node) {
        fout << kv.first << ": " << kv.second << std::endl;
    }
}

void Config::dumpCurrent(std::ostream& out) {
    for (const auto& kv : node) {
        out << kv.first << ": " << kv.second << std::endl;
    }
}
