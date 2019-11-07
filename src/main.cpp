#include <sys/stat.h>
#include <args/args.hxx>
#include <fstream>
#include <iterator>
#include "Config.h"
#include "lua-format.h"

int main(int argc, const char* argv[]) {
    args::ArgumentParser parser("Reformats your Lua source code.", "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Group group(parser, "", args::Group::Validators::DontCare);
    args::Flag verbose(group, "verbose", "Turn on verbose mode", {'v', "verbose"});
    args::Flag inplace(group, "in-place", "Reformats in-place", {'i'});
    args::ValueFlag<string> cFile(parser, "file", "Style config file", {'c', "config"});
    args::PositionalList<string> files(parser, "Lua scripts", "Lua scripts to format");

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        cout << parser;
        return 0;
    } catch (args::ParseError e) {
        cerr << e.what() << std::endl;
        cerr << parser;
        return 1;
    } catch (args::ValidationError e) {
        cerr << e.what() << std::endl;
        cerr << parser;
        return 1;
    }

    string configFileName = args::get(cFile);
    Config config;
    struct stat s;
    if (configFileName != "") {
        if (stat(configFileName.c_str(), &s) == 0) {
            if (!(s.st_mode & S_IFREG)) {
                cerr << configFileName << ": Not a file." << endl;
                return -1;
            }
            if (!(s.st_mode & S_IREAD)) {
                cerr << configFileName << ": No access to read." << endl;
                return -1;
            }
        } else {
            cerr << configFileName << ": No such file." << endl;
            return -1;
        }
        config.readFromFile(configFileName);
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
        }
    } else {
        for (const auto fileName : args::get(files)) {
            if (verbose) {
                cerr << "formatting: " << fileName << endl;
            }
            if (stat(fileName.c_str(), &s) == 0) {
                if (!(s.st_mode & S_IFREG)) {
                    cerr << fileName << ": Not a file." << endl;
                    continue;
                }
                if (!(s.st_mode & S_IREAD)) {
                    cerr << fileName << ": No access to read." << endl;
                    continue;
                }
                if (inplace && !(s.st_mode & S_IWRITE)) {
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
            }
        }
    }

    return 0;
}
