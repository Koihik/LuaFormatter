# LuaFormatter

Reformats your Lua source code.

[![Build Status (Linux / MacOS)](https://travis-ci.org/Koihik/LuaFormatter.svg?branch=master)](https://travis-ci.org/Koihik/LuaFormatter)
[![Build status (Windows)](https://ci.appveyor.com/api/projects/status/to7uvpkdgj96aumg/branch/master?svg=true)](https://ci.appveyor.com/project/Koihik/luaformatter/branch/master)
[![codecov](https://codecov.io/gh/Koihik/LuaFormatter/branch/master/graph/badge.svg)](https://codecov.io/gh/Koihik/LuaFormatter)

## Editor support

* VS Code: https://github.com/Koihik/vscode-lua-format
* Sublime: https://github.com/Koihik/sublime-lua-format
* Vim:     https://github.com/andrejlevkovitch/vim-lua-format

## Install

### LuaRocks

The easiest way is to install is to use [LuaRocks](https://github.com/luarocks/luarocks).

```bash
luarocks install --server=https://luarocks.org/dev luaformatter
```

### Build from source

#### Requirements
* cmake 3.0+
* c++ 17 compiler

#### Steps
```bash
    git clone --recurse-submodules https://github.com/Koihik/LuaFormatter.git
    cd LuaFormatter
    cmake .
    make
    make install
```

## Usage
```
  ./lua-format [Lua scripts...] {OPTIONS}

    Reformats your Lua source code.

  OPTIONS:

      -h, --help                        Display this help menu
      -v, --verbose                     Turn on verbose mode
      -i                                Reformats in-place
      --dump-config                     Dumps the default style used to stdout
      -c[file], --config=[file]         Style config file
      Lua scripts...                    Lua scripts to format
      "--" can be used to terminate flag options and force all following
      arguments to be treated as positional options
```

### Style configure file

See [this file](https://github.com/Koihik/LuaFormatter/blob/master/docs/Style-Config.md)

## Limitations

* Do not work when source file contains syntax error
* Do not support 'Format selection'
