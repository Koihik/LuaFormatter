#include <sstream>
#include <iostream>

#include "../../src/ConfigCheck.h"
#ifdef HAVE_FILESYSTEM_H
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#ifdef HAVE_FILESYSTEM_H
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif

int main(void) {
   std::stringstream ss;
   fs::path current = ".";
   for (auto& entry : fs::directory_iterator(current)) {
      ss << entry.path() << std::endl;
   }
   std::cout << "OK";
   return 0;
}
