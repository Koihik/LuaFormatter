#pragma once

#include <iostream>
#include <string>

// #define LOG_FUNC_VISIT

#ifdef DEBUG

static int logIndentSize = 0;
static std::string LOG_INDENT = "  ";

inline void func_begin(const std::string& funcName) {
    logIndentSize++;
    for (int i = 0; i < logIndentSize; i++) {
        std::cout << LOG_INDENT;
    }
    std::cout << funcName << " begin.." << std::endl;
}

inline void func_end(const std::string& funcName) {
    for (int i = 0; i < logIndentSize; i++) {
        std::cout << LOG_INDENT;
    }
    logIndentSize--;
    std::cout << funcName << " end.." << std::endl;
}

#define LOGVAR(arg)                                                  \
    for (int i = 0; i < logIndentSize; i++) std::cout << LOG_INDENT; \
    std::cout << "" #arg " = " << (arg) << std::endl
#define LOG(arg)                                                     \
    for (int i = 0; i < logIndentSize; i++) std::cout << LOG_INDENT; \
    std::cout << arg << std::endl

    #ifdef LOG_FUNC_VISIT
        #define LOG_FUNCTION_BEGIN() func_begin(__func__)
        #define LOG_FUNCTION_END() func_end(__func__)
    #else
        #define LOG_FUNCTION_BEGIN()
        #define LOG_FUNCTION_END()
    #endif
#else
#define LOG()
#define LOGVAR()
#define LOG_FUNCTION_BEGIN()
#define LOG_FUNCTION_END()
#endif
