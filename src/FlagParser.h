#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class FlagParser {
   public:
    void parse(int argc, const char** argv) {
        string lastKey;
        int remain = 0;
        for (int i = 1; i < argc; i++) {
            string value = argv[i];
            if (value[0] == '-') {
                if (remain > 0) {
                    throw invalid_argument((lastKey + ": Not enough arguements.").c_str());
                }
                if (option_defs_.find(value) == option_defs_.end()) {
                    throw invalid_argument((value + ": Unknown option.").c_str());
                }
                lastKey = value;
                remain = option_defs_[value];
                options_[lastKey] = {};
            } else {
                if (lastKey != "" && remain > 0) {
                    options_[lastKey].push_back(value);
                    remain--;
                } else {
                    values_.push_back(value);
                }
            }
        }
    }

    void define(const string& key, int valueCount) { option_defs_[key] = valueCount; }

    bool hasOption(const string& key) { return options_.count(key); }
    vector<string> getOptions(const string& key) { return options_[key]; }
    string getOption(const string& key) { return options_[key].front(); }
    vector<string> getValues() { return values_; }

   private:
    map<string, int> option_defs_;
    map<string, vector<string>> options_;
    vector<string> values_;
};