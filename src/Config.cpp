#include "Config.h"
#include <fstream>

void trim(string& s) {
    if (!s.empty()) {
        s.erase(0, s.find_first_not_of(" "));
        s.erase(s.find_last_not_of(" ") + 1);
    }
}

Config::Config() {
    _map["indent"] = "    ";
    _map["table_sep"] = ",";
    _map["extra_sep_at_table_end"] = "false";
    _map["chop_down_parameter"] = "50";
    _map["chop_down_block"] = "50";
    _map["chop_down_table"] = "50";
    _map["keep_simple_block_one_line"] = "true";
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
                _map[key] = value;
            }
        }
    }
}
