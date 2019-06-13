#include "Config.h"
#include <fstream>

void trim(string& s) {
    if (!s.empty()) {
        s.erase(0, s.find_first_not_of(" "));
        s.erase(s.find_last_not_of(" ") + 1);
    }
}

Config::Config() {
    map_["column_limit"] = "80";
    map_["indent_width"] = "4";
    map_["use_tab"] = "false";
    map_["tab_width"] = "4";
    map_["continuation_indent_width"] = "4";

    map_["keep_simple_block_one_line"] = "true";

    map_["align_args"] = "true";
    map_["break_after_functioncall_lp"] = "false";
    map_["break_before_functioncall_rp"] = "false";

    map_["align_parameter"] = "true";
    map_["break_after_functiondef_lp"] = "false";
    map_["break_before_functiondef_rp"] = "false";

    map_["align_table_field"] = "true";
    map_["break_after_table_lb"] = "true";
    map_["break_before_table_rb"] = "true";
    map_["chop_down_kv_table"] = "true";
    map_["table_sep"] = ",";
    map_["extra_sep_at_table_end"] = "false";

    map_["break_after_operator"] = "true";
}

void Config::readFromFile(const string& file) {
    ifstream ifs(file);
    string line;
    while (getline(ifs, line)) {
        int idx = line.find(":");
        if (idx != string::npos) {
            string key = line.substr(0, idx);
            string value = line.substr(idx + 1, line.size());
            trim(key);
            trim(value);
            if (key.size() > 0 && key[0] != '#') {
                if (value.size() > 1) {
                    char l = value[0];
                    char r = value[value.size() - 1];
                    if ((l == '"' && r == '"') || (l == '\'' && r == '\'')) {
                        value = value.substr(1, value.size() - 2);
                    }
                }
                map_[key] = value;
            }
        }
    }
}
