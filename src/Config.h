#pragma once

#include <yaml-cpp/yaml.h>

using namespace std;

class Config {
   public:
    Config();
    void readFromFile(const string &file);
    void dumpCurrent(ofstream &fout);
    void dumpCurrent(ostream &out);
    YAML::Node node_;
    template <typename T>
    T get(const char *key) const {
        return node_[key].as<T>();
    }
    template <typename T>
    T get(const char *key) {
        return node_[key].as<T>();
    }
    template <typename T>
    void set(const char *key, T value) {
        node_[key] = value;
    }
    template <typename T>
    void set(const char *key, T value) const {
        node_[key] = value;
    }
};
