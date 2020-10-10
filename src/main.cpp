#include <args/args.hxx>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <iostream>

#include "Config.h"
#include "lua-format.h"

int main(int argc, const char* argv[]) {
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Group dc(parser, "", args::Group::Validators::DontCare);
    args::Flag verbose(dc, "verbose", "Turn on verbose mode", {'v', "verbose"});
    args::Flag inplace(dc, "in-place", "Reformats in-place", {'i'});
    args::Flag dumpcfg(dc, "dump current style", "Dumps the default style used to stdout", {"dump-config"});
    args::ValueFlag<string> cFile(parser, "file", "Style config file", {'c', "config"});
    args::ValueFlag<int> columnlimit(parser, "column limit", "Column limit of one line", {"column-limit"});
    args::ValueFlag<int> indentwidth(parser, "indentation width", "Number of spaces used for indentation",
                                     {"indent-width"});
    args::ValueFlag<int> tabwidth(parser, "tab width", "Number of spaces used per tab", {"tab-width"});
    args::ValueFlag<int> continuationindentwidth(parser, "Continuation indentation width",
                                                 "Indent width for continuations line", {"continuation-indent-width"});
    args::ValueFlag<int> spacesbeforecall(parser, "spaces before call", "Space on function calls",
                                          {"spaces-before-call"});
    args::ValueFlag<int> columntablelimit(parser, "column table limit", "Column limit of each line of a table",
                                          {"column-table-limit"});
    args::ValueFlag<char> tablesep(parser, "table separator", "Character to separate table fields", {"table-sep"});

    args::Group optusetab(parser, "", args::Group::Validators::AtMostOne);
    args::Flag usetab(optusetab, "Tab indentation", "Use tab for indentation", {"use-tab"});
    args::Flag nousetab(optusetab, "Tab indentation", "Do not use tab for indentation", {"no-use-tab"});

    args::Group optcontrolblock(parser, "", args::Group::Validators::AtMostOne);
    args::Flag controlblock(optcontrolblock, "Control block", "keep block in one line",
                            {"keep-simple-control-block-one-line"});
    args::Flag nocontrolblock(optcontrolblock, "Control block", "Do not keep block in one line",
                              {"no-keep-simple-control-block-one-line"});

    args::Group optfunctionline(parser, "", args::Group::Validators::AtMostOne);
    args::Flag functionline(optfunctionline, "function line", "keep function in one line",
                            {"keep-simple-function-one-line"});
    args::Flag nofunctionline(optfunctionline, "function line", "Do not keep function in one line",
                              {"no-keep-simple-function-one-line"});

    args::Group optalignargs(parser, "", args::Group::Validators::AtMostOne);
    args::Flag alignargs(optalignargs, "align args", "Align the arguments", {"align-args"});
    args::Flag noalignargs(optalignargs, "align args", "Do not align the arguments", {"no-align-args"});

    args::Group optfunctioncallLP(parser, "", args::Group::Validators::AtMostOne);
    args::Flag functioncallLP(optfunctioncallLP, "functioncall lp", "Break after '(' of function call",
                              {"break-after-functioncall-lp"});
    args::Flag nofunctioncallLP(optfunctioncallLP, "functioncall lp", "Do not break after '(' of function call",
                                {"no-break-after-functioncall-lp"});

    args::Group optfunctioncallRP(parser, "", args::Group::Validators::AtMostOne);
    args::Flag functioncallRP(optfunctioncallRP, "functioncall rp", "Break before ')' of function call",
                              {"break-before-functioncall-rp"});
    args::Flag nofunctioncallRP(optfunctioncallRP, "functioncall rp", "Do not break before ')' of function call",
                                {"no-break-before-functioncall-rp"});

    args::Group optalignparameter(parser, "", args::Group::Validators::AtMostOne);
    args::Flag alignparameter(optalignparameter, "align parameter", "Align the parameters", {"align-parameter"});
    args::Flag noalignparameter(optalignparameter, "align parameter", "Do not align the parameters",
                                {"no-align-parameter"});

    args::Group optchopdownparameter(parser, "", args::Group::Validators::AtMostOne);
    args::Flag chopdownparameter(optchopdownparameter, "chop down parameter", "Chop down all parameters",
                                 {"chop-down-parameter"});
    args::Flag nochopdownparameter(optchopdownparameter, "chop down parameter", "Do not chop down all parameters",
                                   {"no-chop-down-parameter"});

    args::Group optfunctiondefLP(parser, "", args::Group::Validators::AtMostOne);
    args::Flag functiondefLP(optfunctiondefLP, "functiondef lp", "Break after '(' of function def",
                             {"break-after-functiondef-lp"});
    args::Flag nofunctiondefLP(optfunctiondefLP, "functiondef lp", "Do not break after '(' of function def",
                               {"no-break-after-functiondef-lp"});

    args::Group optfunctiondefRP(parser, "", args::Group::Validators::AtMostOne);
    args::Flag functiondefRP(optfunctiondefRP, "functiondef rp", "Break before ')' of function def",
                             {"break-before-functiondef-rp"});
    args::Flag nofunctiondefRP(optfunctiondefRP, "functiondef rp", "Do not break before ')' of function def",
                               {"no-break-before-functiondef-rp"});

    args::Group optaligntablefield(parser, "", args::Group::Validators::AtMostOne);
    args::Flag aligntablefield(optaligntablefield, "align table field", "Align fields of table", {"align-table-field"});
    args::Flag noaligntablefield(optaligntablefield, "align table field", "Do not align fields of table",
                                 {"no-align-table-field"});

    args::Group opttableLB(parser, "", args::Group::Validators::AtMostOne);
    args::Flag tableLB(opttableLB, "table LB", "Break after '{' of table", {"break-after-table-lb"});
    args::Flag notableLB(opttableLB, "table LB", "Do not break after '{' of table", {"no-break-after-table-lb"});

    args::Group opttableRB(parser, "", args::Group::Validators::AtMostOne);
    args::Flag tableRB(opttableRB, "table RB", "Break before '}' of table", {"break-before-table-rb"});
    args::Flag notableRB(opttableRB, "table RB", "Do not break before '}' of table", {"no-break-before-table-rb"});

    args::Group optchopdowntable(parser, "", args::Group::Validators::AtMostOne);
    args::Flag chopdowntable(optchopdowntable, "chop down table", "Chop down any table", {"chop-down-table"});
    args::Flag nochopdowntable(optchopdowntable, "chop down table", "Do not chop down any table",
                               {"no-chop-down-table"});

    args::Group optchopdownkvtable(parser, "", args::Group::Validators::AtMostOne);
    args::Flag chopdownkvtable(optchopdownkvtable, "chop down table keys", "Chop down table if table contains key",
                               {"chop-down-kv-table"});
    args::Flag nochopdownkvtable(optchopdownkvtable, "chop down table keys",
                                 "Do not chop down table if table contains key", {"no-chop-down-kv-table"});

    args::Group opttableEnd(parser, "", args::Group::Validators::AtMostOne);
    args::Flag tableEnd(opttableEnd, "separator at end", "Add a extra field separator", {"extra-sep-at-table-end"});
    args::Flag notableEnd(opttableEnd, "separator at end", "Do not add a extra field separator",
                          {"no-extra-sep-at-table-end"});

    args::Group optbreakoperator(parser, "", args::Group::Validators::AtMostOne);
    args::Flag breakoperator(optbreakoperator, "break after operator", "Put break after operators",
                             {"break-after-operator"});
    args::Flag nobreakoperator(optbreakoperator, "break after operator", "Do not put break after operators",
                               {"no-break-after-operator"});

    args::Group optDoubleSingle(parser, "", args::Group::Validators::AtMostOne);
    args::Flag DoubleSingle(optDoubleSingle, "double to single quote", "Transform string literals to use single quote",
                            {"double-quote-to-single-quote"});
    args::Flag noDoubleSingle(optDoubleSingle, "double to single quote",
                              "Do not transform string literals to use single quote",
                              {"no-double-quote-to-single-quote"});

    args::Group optSingleDouble(parser, "", args::Group::Validators::AtMostOne);
    args::Flag SingleDouble(optSingleDouble, "single to double quote", "Transform string literals to use double quote",
                            {"single-quote-to-double-quote"});
    args::Flag noSingleDouble(optSingleDouble, "single to double quote",
                              "Do not transform string literals to use double quote",
                              {"no-single-quote-to-double-quote"});

    args::PositionalList<string> files(parser, "Lua scripts", "Lua scripts to format");

    Config config;

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help& e) {
        cout << parser;
        return 0;
    } catch (args::ParseError& e) {
        cerr << e.what() << std::endl;
        cerr << parser;
        return 1;
    } catch (args::ValidationError& e) {
        cerr << e.what() << std::endl;
        cerr << parser;
        return 1;
    }

    if (dumpcfg) {
        config.dumpCurrent(cout);
        return 0;
    }

    if (columnlimit) {
        config.argmap["column_limit"] = args::get(columnlimit);
    }
    if (indentwidth) {
        config.argmap["indent_width"] = args::get(indentwidth);
    }
    if (tabwidth) {
        config.argmap["tab_width"] = args::get(tabwidth);
    }
    if (continuationindentwidth) {
        config.argmap["continuation_indent_width"] = args::get(continuationindentwidth);
    }
    if (spacesbeforecall) {
        config.argmap["spaces_before_call"] = args::get(spacesbeforecall);
    }
    if (columntablelimit) {
        config.argmap["column_table_limit"] = args::get(columntablelimit);
    }
    if (tablesep) {
        config.argmap["table_sep"] = args::get(tablesep);
    }

    if (usetab) {
        config.argmap["use_tab"] = true;
    } else if (nousetab) {
        config.argmap["use_tab"] = false;
    }

    if (controlblock) {
        config.argmap["keep_simple_control_block_one_line"] = true;
    } else if (nocontrolblock) {
        config.argmap["keep_simple_control_block_one_line"] = false;
    }

    if (functionline) {
        config.argmap["keep_simple_function_one_line"] = true;
    } else if (nofunctionline) {
        config.argmap["keep_simple_function_one_line"] = false;
    }

    if (alignargs) {
        config.argmap["align_args"] = true;
    } else if (noalignargs) {
        config.argmap["align_args"] = false;
    }

    if (functioncallLP) {
        config.argmap["break_after_functioncall_lp"] = true;
    } else if (nofunctioncallLP) {
        config.argmap["break_after_functioncall_lp"] = false;
    }

    if (functioncallRP) {
        config.argmap["break_before_functioncall_rp"] = true;
    } else if (nofunctioncallRP) {
        config.argmap["break_before_functioncall_rp"] = false;
    }

    if (alignparameter) {
        config.argmap["align_parameter"] = true;
    } else if (noalignparameter) {
        config.argmap["align_parameter"] = false;
    }

    if (chopdownparameter) {
        config.argmap["chop_down_parameter"] = true;
    } else if (nochopdownparameter) {
        config.argmap["chop_down_parameter"] = false;
    }

    if (functiondefLP) {
        config.argmap["break_after_functiondef_lp"] = true;
    } else if (nofunctiondefLP) {
        config.argmap["break_after_functiondef_lp"] = false;
    }

    if (functiondefRP) {
        config.argmap["break_before_functiondef_rp"] = true;
    } else if (nofunctiondefRP) {
        config.argmap["break_before_functiondef_rp"] = false;
    }

    if (aligntablefield) {
        config.argmap["align_table_field"] = true;
    } else if (noaligntablefield) {
        config.argmap["align_table_field"] = false;
    }

    if (tableLB) {
        config.argmap["break_after_table_lb"] = true;
    } else if (notableLB) {
        config.argmap["break_after_table_lb"] = false;
    }

    if (tableRB) {
        config.argmap["break_before_table_rb"] = true;
    } else if (notableRB) {
        config.argmap["break_before_table_rb"] = false;
    }

    if (chopdowntable) {
        config.argmap["chop_down_table"] = true;
    } else if (nochopdowntable) {
        config.argmap["chop_down_table"] = false;
    }

    if (chopdownkvtable) {
        config.argmap["chop_down_kv_table"] = true;
    } else if (nochopdownkvtable) {
        config.argmap["chop_down_kv_table"] = false;
    }

    if (tableEnd) {
        config.argmap["extra_sep_at_table_end"] = true;
    } else if (notableEnd) {
        config.argmap["extra_sep_at_table_end"] = false;
    }

    if (breakoperator) {
        config.argmap["break_after_operator"] = true;
    } else if (nobreakoperator) {
        config.argmap["break_after_operator"] = false;
    }

    if (DoubleSingle) {
        config.argmap["double_quote_to_single_quote"] = true;
    } else if (noDoubleSingle) {
        config.argmap["double_quote_to_single_quote"] = false;
    }

    if (SingleDouble) {
        config.argmap["single_quote_to_double_quote"] = true;
    } else if (noSingleDouble) {
        config.argmap["single_quote_to_double_quote"] = false;
    }

    string configFileName = args::get(cFile);

    // Automatically look for a .lua-format on the current directory
    if (configFileName.empty()) {
        fs::path current = fs::current_path();
        while (configFileName.empty()) {
            for (auto& entry : fs::directory_iterator(current)) {
                fs::path candidate = entry.path();
                if (candidate.filename() == ".lua-format") configFileName = candidate.string();
            }

            fs::path parent = current.parent_path();
            if (current == parent) {
                break;
            }
            current = parent;
        }
    }

#ifdef __linux__
    if (configFileName.empty()) {
        string conf_dir;
        if (const char* const conf_dir_p = getenv("XDG_CONFIG_HOME")) conf_dir = conf_dir_p;
        if (conf_dir.empty()) {
            string home;
            if (const char* const home_p = getenv("HOME")) home = home_p;
            if (!home.empty()) conf_dir = home + "/.config";
        }
        if (!conf_dir.empty()) {
            string candidate = conf_dir + "/luaformatter/config.yaml";
            if (fs::exists(candidate)) configFileName = candidate;
        }
    }
#endif

    if (configFileName.empty()) {
        if (verbose) cerr << "using default configuration" << endl;
        goto use_default;
    } else {
        if (verbose) cerr << "using configuration file: " << configFileName << endl;
    }

    if (fs::exists(configFileName)) {
        // Keeps the default values in case the yaml is missing a field
        try {
            config.readFromFile(configFileName);
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
            exit(1);
        }
    } else {
        cerr << configFileName << ": No such file." << endl;
        return 1;
    }

use_default:

    if (!config.argmap.empty()) {
        try {
            config.readFromMap(config.argmap);
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
            exit(1);
        }
    }
    bool stdIn = args::get(files).size() == 0;
    if (stdIn) {
        cin >> noskipws;

        istream_iterator<char> it(cin);
        istream_iterator<char> end;
        string input(it, end);

        try {
            string out = lua_format(input, config);

            cout << out;
        } catch (const std::invalid_argument& e) {
            cerr << e.what() << endl;
            return 1;
        }
    } else {
        for (const auto fileName : args::get(files)) {
            if (verbose) {
                cerr << "formatting: " << fileName << endl;
            }
            if (fs::exists(fileName)) {
                fs::file_status status = fs::status(fileName);
                fs::perms perm = status.permissions();

                if (!fs::is_regular_file(status)) {
                    cerr << fileName << ": Not a file." << endl;
                    continue;
                }

                if ((perm & fs::perms::owner_read) == fs::perms::none) {
                    cerr << fileName << ": No access to read." << endl;
                    continue;
                }

                if (inplace && (perm & fs::perms::owner_write) == fs::perms::none) {
                    cerr << fileName << ": No access to write." << endl;
                    continue;
                }
            } else {
                cerr << fileName << ": No such file." << endl;
                continue;
            }

            std::ifstream ifs;
            ifs.open(fileName);

            try {
                string out = lua_format(ifs, config);

                if (!inplace) {
                    cout << out;
                } else {
                    ofstream fout(fileName);
                    fout << out;
                    fout.close();

                    if (verbose) {
                        cerr << "format success: " << fileName << endl;
                    }
                }

            } catch (const std::invalid_argument& e) {
                cerr << e.what() << endl;
                return 1;
            }
        }
    }

    return 0;
}
