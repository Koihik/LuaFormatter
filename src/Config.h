#pragma once

#include <map>
#include <string>

using namespace std;

#define BIND_STR(VARNAME)                                   \
    string VARNAME() const { return _map.at("" #VARNAME); } \
    void VARNAME(const string& s) { _map["" #VARNAME] = s; }

#define BIND_INT(VARNAME)                                      \
    int VARNAME() const { return stoi(_map.at("" #VARNAME)); } \
    void VARNAME(int i) { _map["" #VARNAME] = to_string(i); }

#define BIND_BOOL(VARNAME)                                          \
    bool VARNAME() const { return _map.at("" #VARNAME) == "true"; } \
    void VARNAME(bool x) { _map["" #VARNAME] = x ? "true" : "false"; }

class Config {
   public:
    Config();

    void readFromFile(const string& path);

    BIND_STR(indent);
    BIND_STR(table_sep);
    BIND_BOOL(extra_sep_at_table_end);
    BIND_INT(chop_down_parameter);

   private:
    map<string, string> _map;
};
