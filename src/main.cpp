#include <args/args.hxx>
#include <filesystem>
#include <fstream>
#include <iterator>

#include "Config.h"
#include "lua-format.h"

namespace fs = filesystem;

int main(int argc, const char* argv[]) {
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Group dc(parser, "", args::Group::Validators::DontCare);
    args::Flag verbose(dc, "verbose", "Turn on verbose mode", {'v', "verbose"});
    args::Flag inplace(dc, "in-place", "Reformats in-place", {'i'});
    args::Flag dumpcfg(dc, "dump current style", "Dumps the default style used to stdout", {"dump-config"});
    args::ValueFlag<string> cFile(parser, "file", "Style config file", {'c', "config"});
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

    string configFileName = args::get(cFile);

    // Automatically look for a .lua-format on the current directory
    if (configFileName.empty()) {
        fs::path current = ".";
        for (auto& entry : fs::directory_iterator(current)) {
            fs::path candidate = entry.path();
            if (candidate.filename() == ".lua-format") configFileName = candidate.string();
        }
    }

    if (configFileName.empty()) {
        if (verbose) cerr << "using default configuration" << endl;
        goto use_default;
    }

    if (fs::exists(configFileName)) {
        // Keeps the default values in case the yaml is missing a field
        config.readFromFile(configFileName);
    } else {
        cerr << configFileName << ": No such file." << endl;
        return 1;
    }

use_default:

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
