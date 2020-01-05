#include "Config.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = filesystem;

Config::Config() {
    // Defaul configuration
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

    node_["break_after_operator"] = true;

    node_["double_quote_to_single_quote"] = false;
    node_["single_quote_to_double_quote"] = false;
}

void Config::readFromFile(const string& file) {
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
    for (auto kv : n) {
        auto key = kv.first.as<string>();
        if (node_[key]) {
            node_[key] = kv.second;
        }
    }
}

void Config::dumpCurrent(ofstream& fout) { fout << node_ << endl; }

void Config::dumpCurrent(ostream& out) { out << node_ << endl; }
