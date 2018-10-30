#include <sys/stat.h>
#include <fstream>
#include "Config.h"
#include "lua-format.h"

int main(int argc, const char* argv[]) {
    // FIXME: import some parse flag lib
    if (argc == 1) {
        cerr << "No input file specified." << endl;
        return -1;
    }
    if (argc != 2 && argc != 4) {
        cerr << "Arguements count err." << endl;
        return -1;
    }

    string configFileName;
    string fileName;
    if (argc == 2) {
        fileName = argv[1];
    } else {
        string cfg = argv[1];
        if (cfg != "-c" && cfg != "-config" && cfg != "--config") {
            cerr << "Unknown flag: " << argv[1] << endl;
            return -1;
        }
        configFileName = argv[2];
        fileName = argv[3];
    }

    struct stat s;
    if (configFileName != "") {
        if (stat(configFileName.c_str(), &s) == 0) {
            if (!(s.st_mode & S_IFREG)) {
                cerr << configFileName << ": Not a file." << endl;
                return -1;
            }
        } else {
            cerr << configFileName << ": No such file or no access to write." << endl;
            return -1;
        }
    }
    if (stat(fileName.c_str(), &s) == 0) {
        if (!(s.st_mode & S_IFREG)) {
            cerr << fileName << ": Not a file." << endl;
            return -1;
        }
    } else {
        cerr << fileName << ": No such file or no access to write." << endl;
        return -1;
    }

    Config config;

    if (configFileName != "") {
        config.readFromFile(configFileName);
    }

    std::ifstream ifs;
    ifs.open(fileName);
    string out = lua_format(ifs, config);

    ofstream fout(fileName);
    fout << out;
    fout.close();
    return 0;
}