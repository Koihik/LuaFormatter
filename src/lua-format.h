#pragma once

#include <iostream>
#include <string>

#include "Config.h"

using namespace std;

string lua_format(const string& input, const Config& config);
string lua_format(istream& stream, const Config& config);