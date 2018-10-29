#pragma once

#include <map>
#include <string>

using namespace std;

#define BIND_STR(VARNAME)                          \
    string VARNAME() { return _map["" #VARNAME]; } \
    void VARNAME(const string& s) { _map["" #VARNAME] = s; }

#define BIND_INT(VARNAME)                             \
    int VARNAME() { return stoi(_map["" #VARNAME]); } \
    void VARNAME(int i) { _map["" #VARNAME] = to_string(i); }

#define BIND_BOOL(VARNAME)                                 \
    bool VARNAME() { return _map["" #VARNAME] == "true"; } \
    void VARNAME(bool x) { _map["" #VARNAME] = x ? "true" : "false"; }

class Config {
   public:
    Config();

    void readFromFile(const string& path);

    BIND_STR(indent);
    BIND_STR(table_sep);
    BIND_BOOL(extra_sep_at_table_end);
    BIND_BOOL(keep_simple_function_one_line);
    BIND_BOOL(keep_simple_table_one_line);

   private:
    map<string, string> _map;
};
