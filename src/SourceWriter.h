#pragma once

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class SourceWriter {
   private:
    stringstream ss_;
    vector<int> columns_;
    int lines_ = 1;
    bool volatile_ = false;
    char lastChar;

   public:
    SourceWriter() { columns_.push_back(0); }
    ~SourceWriter() {}

    SourceWriter& operator<<(const string& str) {
        for (char c : str) {
            if (c == '\n') {
                lines_++;
                columns_.push_back(0);
            } else {
                columns_[columns_.size() - 1]++;
            }
            lastChar = c;
            if (!volatile_) {
                ss_ << c;
            }
        }
        return *this;
    }

    string str() { return ss_.str(); }
    stringstream& ss() { return ss_; }
    const vector<int>& columns() { return columns_; }
    int lines() { return lines_; }
    void set_volatile(bool v) { volatile_ = v; }
    bool isLastCharWhiteSpace() { return ' ' == lastChar; }
};