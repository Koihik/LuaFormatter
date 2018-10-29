#include "Config.h"
#include <fstream>

string& trim(string& s) {
    if (s.empty()) {
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

Config::Config() {
    _map["indent"] = "    ";
    _map["table_sep"] = ",";
    _map["extra_sep_at_table_end"] = "false";
    _map["keep_simple_function_one_line"] = "true";
    _map["keep_simple_table_one_line"] = "true";
}

void Config::readFromFile(const string& file) {
    ifstream ifs(file);
    string line;
    while (getline(ifs, line)) {
        int idx = line.find(":");
        if (idx != string::npos) {
            string key = line.substr(0, idx);
            string value = line.substr(idx + 1, line.size());
            key = trim(key);
            value = trim(value);
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
