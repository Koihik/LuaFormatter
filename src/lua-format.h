#pragma once

#include <iostream>
#include <string>

#include "Config.h"

std::string lua_format(const std::string& input, const Config& config);
std::string lua_format(std::istream& stream, const Config& config);
