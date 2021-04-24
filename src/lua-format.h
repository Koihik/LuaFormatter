#pragma once

#include <iostream>
#include <string>

#include "Config.h"

std::string lua_format(const std::string& input, const Config& config);
std::string lua_format(std::istream& stream, const Config& config);

/// Return the line separator (LF, CRLF, CR) that appears the most
/// When ties or the input has no newline, return in the order of LF > CRLF > CR
std::string get_line_separator(const std::string& input);

/// Return input with all line separator replaced with the specified one
std::string convert_line_separator(const std::string& input, const std::string& line_sep);
