#include <sys/stat.h>
#include <fstream>
#include <iterator>
#include "Config.h"
#include "FlagParser.h"
#include "lua-format.h"

int main(int argc, const char* argv[]) {
    FlagParser parser;
    parser.define("-c", 1);
    parser.define("-v", 0);
    parser.define("-si", 0);
    parser.define("-so", 0);

    parser.parse(argc, argv);

    bool verbose = parser.hasOption("-v");
    bool stdIn = parser.hasOption("-si");
    bool stdOut = parser.hasOption("-so") || stdIn;

    string configFileName;

    if (parser.hasOption("-c")) {
        configFileName = parser.getOption("-c");
    }

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
        vector<string> files = parser.getValues();
        if (files.size() == 0) {
            cerr << "No input file specified.";
        }
        for (string fileName : files) {
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
                if (!stdOut && !(s.st_mode & S_IWRITE)) {
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

                if (stdOut) {
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