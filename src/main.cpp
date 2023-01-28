#include <args/args.hxx>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>

#include "Config.h"
#include "lua-format.h"

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
int setBinaryMode(FILE *file) {
    return _setmode(_fileno(file), _O_BINARY);
}
#else
int setBinaryMode(FILE*) {
    return 0;
}
#endif

bool file_is_usable(const std::string& fileName, const bool inplace) {
    if (!fs::exists(fileName)) {
        std::cerr << fileName << ": No such file." << std::endl;
        return false;
    }

    fs::file_status status = fs::status(fileName);
    fs::perms perm = status.permissions();

    if (!fs::is_regular_file(status)) {
        std::cerr << fileName << ": Not a file." << std::endl;
        return false;
    }

    if ((perm & fs::perms::owner_read) == fs::perms::none) {
        std::cerr << fileName << ": No access to read." << std::endl;
        return false;
    }

    if (inplace && (perm & fs::perms::owner_write) == fs::perms::none) {
        std::cerr << fileName << ": No access to write." << std::endl;
        return false;
    }

    return true;
}

int main(int argc, const char* argv[]) {
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Group dc(parser, "", args::Group::Validators::DontCare);
    args::Flag verbose(dc, "verbose", "Turn on verbose mode", {'v', "verbose"});
    args::Flag version(dc, "version", "Display the version and exit", {'e', "version"});
    args::Flag inplace(dc, "in-place", "Reformats in-place", {'i', "in-place"});
    args::Flag check(dc, "check", "Non-zero return if formatting is needed", {"check"});
    args::Flag dumpcfg(dc, "dump current style", "Dumps the default style used to stdout", {"dump-config"});
    args::ValueFlag<std::string> cFile(parser, "file", "Style config file", {'c', "config"});
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
    args::ValueFlag<int> columntablelimitkv(parser, "column table kv limit", "The column limit of each line of a k = v table. Default value the same as column_table_limit value.",
                                          {"column_table_limit_kv"});
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

    args::Group optspacesinsidefunctiondefparens(parser, "", args::Group::Validators::AtMostOne);
    args::Flag spacesinsidefunctiondefparens(optspacesinsidefunctiondefparens, "spaces inside function header parens",
                               "Put spaces on the inside of parens in function headers", {"spaces-inside-functiondef-parens"});
    args::Flag nospacesinsidefunctiondefparens(optspacesinsidefunctiondefparens, "spaces inside function header parens",
                               "Do not put spaces on the inside of parens in function headers", {"no-spaces-inside-functiondef-parens"});

    args::Group optspacesinsidefunctioncallparens(parser, "", args::Group::Validators::AtMostOne);
    args::Flag spacesinsidefunctioncallparens(optspacesinsidefunctioncallparens, "spaces inside function call parens",
                               "Put spaces on the inside of parens in function calls", {"spaces-inside-functioncall-parens"});
    args::Flag nospacesinsidefunctioncallparens(optspacesinsidefunctioncallparens, "spaces inside function call parens",
                               "Do not put spaces on the inside of parens in function calls", {"no-spaces-inside-functioncall-parens"});

    args::Group optspacesinsidetablebraces(parser, "", args::Group::Validators::AtMostOne);
    args::Flag spacesinsidetablebraces(optspacesinsidetablebraces, "spaces inside table constructor braces",
                               "Put spaces on the inside of braces in table constructors", {"spaces-inside-table-braces"});
    args::Flag nospacesinsidetablebraces(optspacesinsidetablebraces, "spaces inside table constructor braces",
                               "Do not put spaces on the inside of braces in table constructors", {"no-spaces-inside-table-braces"});

    args::Group optspacesaroundequalsinfield(parser, "", args::Group::Validators::AtMostOne);
    args::Flag spacesaroundequalsinfield(optspacesaroundequalsinfield, "spaces around equals sign in key/value fields",
                               "Put spaces around the equal sign in key/value fields", {"spaces-around-equals-in-field"});
    args::Flag nospacesaroundequalsinfield(optspacesaroundequalsinfield, "spaces around equals sign in key/value fields",
                               "Do not put spaces around the equal sign in key/value fields", {"no-spaces-around-equals-in-field"});
    args::ValueFlag<int> linebreaksafterfunctionbody(parser, "line breaks after function body", "Line breaks after function body", {"line-breaks-after-function-body"});

    args::ValueFlag<std::string> lineseparator(parser, "line separator",
                                               "input(determined by the input content), "
                                               "os(Use line ending of the current Operating system), "
                                               "lf(Unix style \"\\n\"), "
                                               "crlf(Windows style \"\\r\\n\"), "
                                               "cr(classic Max style \"\\r\")",
                                               {"line-separator"});

    args::PositionalList<std::string> files(parser, "Lua scripts", "Lua scripts to format");

    Config config;

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help&) {
        std::cout << parser;
        std::cout << "Version:  " << LUA_FORMAT_VERSION << std::endl;
        return 0;
    } catch (args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    } catch (args::ValidationError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    std::map<std::string, std::any> argmap;

    if (columnlimit) {
        argmap["column_limit"] = args::get(columnlimit);
    }
    if (indentwidth) {
        argmap["indent_width"] = args::get(indentwidth);
    }
    if (tabwidth) {
        argmap["tab_width"] = args::get(tabwidth);
    }
    if (continuationindentwidth) {
        argmap["continuation_indent_width"] = args::get(continuationindentwidth);
    }
    if (spacesbeforecall) {
        argmap["spaces_before_call"] = args::get(spacesbeforecall);
    }
    if (columntablelimit) {
        argmap["column_table_limit"] = args::get(columntablelimit);
    }
    if (columntablelimitkv) {
        argmap["column_table_limit_kv"] = args::get(columntablelimitkv);
    }
    if (tablesep) {
        argmap["table_sep"] = args::get(tablesep);
    }

    if (usetab) {
        argmap["use_tab"] = true;
    } else if (nousetab) {
        argmap["use_tab"] = false;
    }

    if (controlblock) {
        argmap["keep_simple_control_block_one_line"] = true;
    } else if (nocontrolblock) {
        argmap["keep_simple_control_block_one_line"] = false;
    }

    if (functionline) {
        argmap["keep_simple_function_one_line"] = true;
    } else if (nofunctionline) {
        argmap["keep_simple_function_one_line"] = false;
    }

    if (alignargs) {
        argmap["align_args"] = true;
    } else if (noalignargs) {
        argmap["align_args"] = false;
    }

    if (functioncallLP) {
        argmap["break_after_functioncall_lp"] = true;
    } else if (nofunctioncallLP) {
        argmap["break_after_functioncall_lp"] = false;
    }

    if (functioncallRP) {
        argmap["break_before_functioncall_rp"] = true;
    } else if (nofunctioncallRP) {
        argmap["break_before_functioncall_rp"] = false;
    }

    if (alignparameter) {
        argmap["align_parameter"] = true;
    } else if (noalignparameter) {
        argmap["align_parameter"] = false;
    }

    if (chopdownparameter) {
        argmap["chop_down_parameter"] = true;
    } else if (nochopdownparameter) {
        argmap["chop_down_parameter"] = false;
    }

    if (functiondefLP) {
        argmap["break_after_functiondef_lp"] = true;
    } else if (nofunctiondefLP) {
        argmap["break_after_functiondef_lp"] = false;
    }

    if (functiondefRP) {
        argmap["break_before_functiondef_rp"] = true;
    } else if (nofunctiondefRP) {
        argmap["break_before_functiondef_rp"] = false;
    }

    if (aligntablefield) {
        argmap["align_table_field"] = true;
    } else if (noaligntablefield) {
        argmap["align_table_field"] = false;
    }

    if (tableLB) {
        argmap["break_after_table_lb"] = true;
    } else if (notableLB) {
        argmap["break_after_table_lb"] = false;
    }

    if (tableRB) {
        argmap["break_before_table_rb"] = true;
    } else if (notableRB) {
        argmap["break_before_table_rb"] = false;
    }

    if (chopdowntable) {
        argmap["chop_down_table"] = true;
    } else if (nochopdowntable) {
        argmap["chop_down_table"] = false;
    }

    if (chopdownkvtable) {
        argmap["chop_down_kv_table"] = true;
    } else if (nochopdownkvtable) {
        argmap["chop_down_kv_table"] = false;
    }

    if (tableEnd) {
        argmap["extra_sep_at_table_end"] = true;
    } else if (notableEnd) {
        argmap["extra_sep_at_table_end"] = false;
    }

    if (breakoperator) {
        argmap["break_after_operator"] = true;
    } else if (nobreakoperator) {
        argmap["break_after_operator"] = false;
    }

    if (DoubleSingle) {
        argmap["double_quote_to_single_quote"] = true;
    } else if (noDoubleSingle) {
        argmap["double_quote_to_single_quote"] = false;
    }

    if (SingleDouble) {
        argmap["single_quote_to_double_quote"] = true;
    } else if (noSingleDouble) {
        argmap["single_quote_to_double_quote"] = false;
    }

    if (spacesinsidefunctiondefparens) {
      argmap["spaces_inside_functiondef_parens"] = true;
    } else if (nospacesinsidefunctiondefparens) {
      argmap["spaces_inside_functiondef_parens"] = false;
    }

    if (spacesinsidefunctioncallparens) {
      argmap["spaces_inside_functioncall_parens"] = true;
    } else if (nospacesinsidefunctioncallparens) {
      argmap["spaces_inside_functioncall_parens"] = false;
    }

    if (spacesinsidetablebraces) {
      argmap["spaces_inside_table_braces"] = true;
    } else if (nospacesinsidetablebraces) {
      argmap["spaces_inside_table_braces"] = false;
    }

    if (spacesaroundequalsinfield) {
      argmap["spaces_around_equals_in_field"] = true;
    } else if (nospacesaroundequalsinfield) {
      argmap["spaces_around_equals_in_field"] = false;
    }

    if (linebreaksafterfunctionbody) {
        argmap["line_breaks_after_function_body"] = args::get(linebreaksafterfunctionbody);
    }

    if (lineseparator) {
        argmap["line_separator"] = args::get(lineseparator);
    }

    std::string configFileName = args::get(cFile);

    // Automatically look for a .lua-format on the current directory
    if (configFileName.empty()) {
        fs::path current = fs::current_path();
        while (configFileName.empty()) {
            for (const auto& entry : fs::directory_iterator(current)) {
                const fs::path& candidate = entry.path();
                if (candidate.filename() == ".lua-format") {
                    configFileName = candidate.string();
                }
            }

            fs::path parent = current.parent_path();
            if (current == parent) {
                break;
            }
            current = parent;
        }
    }

#if defined __unix__ || defined __APPLE__
    if (configFileName.empty()) {
        std::string conf_dir;
        if (const char* const conf_dir_p = getenv("XDG_CONFIG_HOME")) {
            conf_dir = conf_dir_p;
        }
        if (conf_dir.empty()) {
            std::string home;
            if (const char* const home_p = getenv("HOME")) {
                home = home_p;
            }
            if (!home.empty()) {
                conf_dir = home + "/.config";
            }
        }
        if (!conf_dir.empty()) {
            std::string candidate = conf_dir + "/luaformatter/config.yaml";
            if (fs::exists(candidate)) {
                configFileName = candidate;
            }
        }
    }
#endif

    if (configFileName.empty()) {
        if (verbose) {
            std::cerr << "using default configuration" << std::endl;
        }
    } else {
        if (verbose) {
            std::cerr << "using configuration file: " << configFileName << std::endl;
        }

        if (fs::exists(configFileName)) {
            // Keeps the default values in case the yaml is missing a field
            try {
                config.readFromFile(configFileName);
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
                exit(1);
            }
        } else {
            std::cerr << configFileName << ": No such file." << std::endl;
            return 1;
        }
    }

    // Command line overwrites the config file
    if (!argmap.empty()) {
        try {
            config.readFromMap(argmap);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            exit(1);
        }
    }

    if (dumpcfg) {
        config.dumpCurrent(std::cout);
        return 0;
    }

    if (version) {
        std::cout << "Version:  " << LUA_FORMAT_VERSION << std::endl;
        return 0;
    }

    setBinaryMode(stdout);

    bool would_format = false;

    bool stdIn = args::get(files).empty();
    if (stdIn) {
        std::istreambuf_iterator<char> it(std::cin);
        std::istreambuf_iterator<char> end;
        std::string input(it, end);

        try {
            std::string out = lua_format(input, config);

            if (check) {
                would_format = input != out;
            } else {
                std::cout << out;
                return 0;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }

    for (const auto& fileName : args::get(files)) {
        if (verbose) {
            std::cerr << "formatting: " << fileName << std::endl;
        }
        if (!file_is_usable(fileName, inplace)) {
            continue;
        }

        std::ifstream ifs(fileName, std::ifstream::binary);
        std::string input((std::istreambuf_iterator<char>(ifs)), {});

        try {
            std::string out = lua_format(input, config);

            if (check) {
                if (input != out) {
                    std::cout << fileName << std::endl;
                    would_format = true;
                }
                continue;
            }
            if (!inplace) {
                std::cout << out;
            } else {
                std::ofstream fout(fileName, std::ofstream::binary);
                fout << out;
                fout.close();

                if (verbose) {
                    std::cerr << "format success: " << fileName << std::endl;
                }
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }

    if (check && would_format) {
        return 2;
    }
    return 0;
}
