#include "Config.h"

#include <fstream>

Config::Config() {
    // Defaul configuration
    node_["column_limit"] = 80;
    node_["indent_width"] = 4;
    node_["use_tab"] = false;
    node_["tab_width"] = 4;
    node_["continuation_indent_width"] = 4;

    node_["keep_simple_block_one_line"] = true;

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
    node_["chop_down_kv_table"] = true;
    node_["table_sep"] = ",";
    node_["extra_sep_at_table_end"] = false;

    node_["break_after_operator"] = true;
}

void Config::readFromFile(const string& file) {
    YAML::Node n = YAML::LoadFile(file);

    // Keys are always strings
    for (auto kv : n) {
        auto key = kv.first.as<string>();
        if (node_[key]) {
            node_[key] = kv.second;
        }
    }
}

void Config::dumpCurrent(ofstream& fout) { fout << node_; }

void Config::dumpCurrent(ostream& out) { out << node_; }
