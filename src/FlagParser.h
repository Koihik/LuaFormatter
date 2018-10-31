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
                if (_optionDefs.find(value) == _optionDefs.end()) {
                    throw invalid_argument((value + ": Unknown option.").c_str());
                }
                lastKey = value;
                remain = _optionDefs[value];
                _options[lastKey] = {};
            } else {
                if (lastKey != "" && remain > 0) {
                    _options[lastKey].push_back(value);
                    remain--;
                } else {
                    _values.push_back(value);
                }
            }
        }
    }

    void define(const string& key, int valueCount) { _optionDefs[key] = valueCount; }

    bool hasOption(const string& key) { return _options.count(key); }
    vector<string> getOptions(const string& key) { return _options[key]; }
    string getOption(const string& key) { return _options[key].front(); }
    vector<string> getValues() { return _values; }

   private:
    map<string, int> _optionDefs;
    map<string, vector<string>> _options;
    vector<string> _values;
};