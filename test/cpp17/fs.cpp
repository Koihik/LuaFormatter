#include <sstream>
#include <iostream>
#include <filesystem>

int main(void) {
   std::stringstream ss;
   std::filesystem::path current = ".";
   for (auto& entry : std::filesystem::directory_iterator(current)) {
      ss << entry.path() << std::endl;
   }
   std::cout << "OK";
   return 0;
}
