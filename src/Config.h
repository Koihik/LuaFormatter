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

    BIND_INT(column_limit);
    BIND_INT(indent_width);
    BIND_BOOL(use_tab);
    BIND_INT(tab_width);
    BIND_INT(continuation_indent_width);
    
    BIND_BOOL(keep_simple_block_one_line);

    BIND_BOOL(align_args);
    BIND_BOOL(break_after_functioncall_lp);
    BIND_BOOL(break_before_functioncall_rp);

    BIND_BOOL(align_parameter);
    BIND_BOOL(break_after_functiondef_lp);
    BIND_BOOL(break_before_functiondef_rp);

    BIND_BOOL(align_table_field);
    BIND_BOOL(break_after_table_lb);
    BIND_BOOL(break_before_table_rb);
    BIND_BOOL(chop_down_kv_table);
    BIND_STR(table_sep);
    BIND_BOOL(extra_sep_at_table_end);

    BIND_BOOL(break_after_operator);

   private:
    map<string, string> map_;
};
