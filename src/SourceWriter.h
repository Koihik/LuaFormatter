#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class SourceWriter {
   private:
    stringstream ss_;
    vector<int> columns_;
    int lines_ = 1;
    bool volatile_ = false;
    char lastChar_;
    int initialColumn_ = 0;

   public:
    SourceWriter() { columns_.push_back(0); }
    SourceWriter(int initialColumn) {
        initialColumn_ = initialColumn;
        columns_.push_back(initialColumn);
    }
    ~SourceWriter() {}

    SourceWriter& operator<<(const string& str) {
        for (char c : str) {
            if (c == '\n') {
                lines_++;
                columns_.push_back(0);
            } else {
                columns_[columns_.size() - 1]++;
            }
            lastChar_ = c;
            if (!volatile_) {
                ss_ << c;
            }
        }
        return *this;
    }

    string str() { return ss_.str(); }
    stringstream& ss() { return ss_; }
    const vector<int>& columns() { return columns_; }
    int firstLineColumn() { return columns_.front() - initialColumn_; }
    int lines() { return lines_; }
    void set_volatile(bool v) { volatile_ = v; }
    bool isLastCharWhiteSpace() { return ' ' == lastChar_; }
};