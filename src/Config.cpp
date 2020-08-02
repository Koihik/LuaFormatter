#include "Config.h"

#include <assert.h>

#include <any>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

namespace fs = filesystem;

Config::Config() {
    // Default configuration
    node_["column_limit"] = 80;
    node_["indent_width"] = 4;
    node_["use_tab"] = false;
    node_["tab_width"] = 4;
    node_["continuation_indent_width"] = 4;
    node_["spaces_before_call"] = 1;

    node_["keep_simple_control_block_one_line"] = true;
    node_["keep_simple_function_one_line"] = true;

    node_["align_args"] = true;
    node_["break_after_functioncall_lp"] = false;
    node_["break_before_functioncall_rp"] = false;

    node_["align_parameter"] = true;
    node_["chop_down_parameter"] = false;
    node_["break_after_functiondef_lp"] = false;
    node_["break_before_functiondef_rp"] = false;

    node_["align_table_field"] = true;
    node_["break_after_table_lb"] = true;
    node_["break_before_table_rb"] = true;
    node_["chop_down_table"] = false;
    node_["chop_down_kv_table"] = true;
    node_["table_sep"] = ",";
    node_["extra_sep_at_table_end"] = false;
    node_["column_table_limit"] = node_["column_limit"];

    node_["break_after_operator"] = true;

    node_["double_quote_to_single_quote"] = false;
    node_["single_quote_to_double_quote"] = false;

    // Validators
    // validate integer without 0s as configuration value
    auto validate_integer = [](std::string key, std::any elem) {
        int value = std::any_cast<int>(elem);
        if (value > 0) {
            return value;
        }
        std::cerr << "[ERROR] Processing " << key << " failed" << std::endl;
        throw domain_error("[ERROR] Configuration value is out of range. Must be a positive integer greater than 0.");
    };

    // validate integer with 0s as configuration value
    auto validate_integer_zero = [](std::string key, std::any elem) {
        int value = std::any_cast<int>(elem);
        if (value >= 0) {
            return value;
        }
        std::cerr << "[ERROR] Processing " << key << " failed" << std::endl;
        throw domain_error("[ERROR] Configuration value is out of range. Must be a positive integer.");
    };
    auto validate_boolean = [](std::string key, std::any elem) {
        bool value = std::any_cast<bool>(elem);
        return value;
    };
    auto validate_character = [](std::string key, std::any elem) {
        char value = std::any_cast<char>(elem);
        return value;
    };
    validators["spaces_before_call"] = validate_integer_zero;
    validators["column_limit"] = validate_integer;
    validators["indent_width"] = validate_integer_zero;
    validators["tab_width"] = validate_integer;
    validators["continuation_indent_width"] = validate_integer_zero;
    validators["column_table_limit"] = validate_integer;
    validators["use_tab"] = validate_boolean;
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
    validators["double_quote_to_single_quote"] = validate_boolean;
    validators["single_quote_to_double_quote"] = validate_boolean;
    validators["table_sep"] = validate_character;

    // DataType of every configuration field
    datatype["spaces_before_call"] = 'i';
    datatype["column_limit"] = 'i';
    datatype["indent_width"] = 'i';
    datatype["tab_width"] = 'i';
    datatype["continuation_indent_width"] = 'i';
    datatype["column_table_limit"] = 'i';
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
}

void Config::readFromFile(const string &file) {
    fs::file_status status = fs::status(file);
    fs::perms perm = status.permissions();

    if (!fs::is_regular_file(status)) {
        cerr << file << ": Not a file." << endl;
        exit(-1);
    }

    if ((perm & fs::perms::owner_read) == fs::perms::none) {
        cerr << file << ": No access to read." << endl;
        exit(-1);
    }

    YAML::Node n = YAML::LoadFile(file);

    // Keys are always strings
    bool given = false;
    string CTL = "column_table_limit";
    string CL = "column_limit";
    for (auto kv : n) {
        auto key = kv.first.as<string>();
        if (node_[key]) {
            switch (datatype[key]) {
                case 'i': {
                    auto value = kv.second.as<int>();
                    node_[key] = std::any_cast<int>(validators[key](key, value));
                    break;
                }
                case 'b': {
                    auto value = kv.second.as<bool>();
                    node_[key] = std::any_cast<bool>(validators[key](key, value));
                    break;
                }
                case 'c': {
                    auto value = kv.second.as<char>();
                    node_[key] = std::any_cast<char>(validators[key](key, value));
                    break;
                }
            }
        }
        if (key == CTL) {
            given = true;
        }
    }
    if (!given) {
        node_[CTL] = node_[CL];
    }
}

void Config::readFromMap(map<string, any> &mp) {
    bool given = false;
    string CTL = "column_table_limit";
    string CL = "column_limit";
    for (auto kv : mp) {
        auto key = kv.first;
        if (node_[key]) {
            switch (datatype[key]) {
                case 'i': {
                    assert(strcmp(kv.second.type().name(), "i") == 0);
                    node_[key] = std::any_cast<int>(validators[key](key, kv.second));
                    break;
                }
                case 'b': {
                    assert(strcmp(kv.second.type().name(), "b") == 0);
                    node_[key] = std::any_cast<bool>(validators[key](key, kv.second));
                    break;
                }
                case 'c': {
                    assert(strcmp(kv.second.type().name(), "c") == 0);
                    node_[key] = std::any_cast<char>(validators[key](key, kv.second));
                    break;
                }
            }
        }
        if (key == CTL) {
            given = true;
        }
    }
    if (!given) {
        node_[CTL] = node_[CL];
    }
}

void Config::dumpCurrent(ofstream &fout) { fout << node_ << endl; }

void Config::dumpCurrent(ostream &out) { out << node_ << endl; }
