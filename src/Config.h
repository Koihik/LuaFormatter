#pragma once

#include <map>
#include <string>

using namespace std;

#define BIND_STR(VARNAME)                                   \
    string VARNAME() const { return map_.at("" #VARNAME); } \
    void VARNAME(const string& s) { map_["" #VARNAME] = s; }

#define BIND_INT(VARNAME)                                      \
    int VARNAME() const { return stoi(map_.at("" #VARNAME)); } \
    void VARNAME(int i) { map_["" #VARNAME] = to_string(i); }

#define BIND_BOOL(VARNAME)                                          \
    bool VARNAME() const { return map_.at("" #VARNAME) == "true"; } \
    void VARNAME(bool x) { map_["" #VARNAME] = x ? "true" : "false"; }

class Config {
   public:
    Config();

    void readFromFile(const string& path);

    BIND_STR(indent);
    BIND_STR(table_sep);
    BIND_BOOL(extra_sep_at_table_end);
    BIND_INT(chop_down_parameter);
    BIND_INT(chop_down_block);
    BIND_INT(chop_down_table);
    BIND_BOOL(keep_simple_block_one_line);
    BIND_INT(columns_limit);

   private:
    map<string, string> map_;
};
