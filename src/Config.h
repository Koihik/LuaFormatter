#pragma once

#include <yaml-cpp/yaml.h>

#include "ConfigCheck.h"
#ifdef HAVE_FILESYSTEM_H
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include <any>
#include <functional>

using Validator = std::function<std::any(const std::string &, std::any)>;

#ifdef HAVE_FILESYSTEM_H
namespace fs = std::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif

class Config {
    YAML::Node node;
    std::map<std::string, Validator> validators;
    std::map<std::string, char> datatype;

   public:
    Config();
    void readFromFile(const std::string &file);
    void dumpCurrent(std::ofstream &fout);
    void dumpCurrent(std::ostream &out);
    void readFromMap(std::map<std::string, std::any> &mp);
    template <typename T>
    T get(const char *key) const {
        return node[key].as<T>();
    }
    template <typename T>
    T get(const char *key) {
        return node[key].as<T>();
    }
    template <typename T>
    void set(const char *key, T value) {
        node[key] = value;
    }
    template <typename T>
    void set(const char *key, T value) const {
        node[key] = value;
    }
};
