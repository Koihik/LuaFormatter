#pragma once

#include <iostream>
#include <string>

#include "Config.h"

using namespace std;

string lua_format(const string& input, Config& config);
string lua_format(istream& stream, Config& config);