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

    std::ifstream ifs;
    ifs.open(argv[1]);
    cout << lua_format(ifs);
    return 0;
}