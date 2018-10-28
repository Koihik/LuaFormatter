#include <sys/stat.h>
#include <fstream>
#include "lua-format.h"

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        cerr << "No input file specified." << endl;
        return -1;
    }
    if (argc > 2) {
        cerr << "More than one file specified." << endl;
        return -1;
    }

    string fileName = argv[1];

    struct stat s;
    if (stat(fileName.c_str(), &s) == 0) {
        if (!(s.st_mode & S_IFREG)) {
            cerr << fileName << ": Not a file." << endl;
            return -1;
        }
    } else {
        cerr << fileName << ": No such file or no access to write." << endl;
        return -1;
    }

    std::ifstream ifs;
    ifs.open(fileName);
    string out = lua_format(ifs);

    ofstream fout(fileName);
    fout << out;
    fout.close();
    return 0;
}