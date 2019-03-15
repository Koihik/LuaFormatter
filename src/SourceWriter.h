#pragma once

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class SourceWriter {
   private:
    stringstream ss_;
    int columns_ = 0;
    int lines_ = 0;

   public:
    SourceWriter() {}
    ~SourceWriter() {}

    SourceWriter& operator<<(const string& str) {
        for (char c : str) {
            if (c == '\n') {
                lines_++;
                columns_ = 0;
            } else {
                columns_++;
            }
            ss_ << c;
        }
        return *this;
    }

    string str() { return ss_.str(); }
    stringstream& ss() { return ss_; }
    int columns() { return columns_; }
    int lines() { return lines_; }
};