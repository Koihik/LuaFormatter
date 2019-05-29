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
Build from source.

### Requirements
* cmake 3.0+
* c++ 11 compiler

### Steps
```bash
    git clone https://github.com/Koihik/LuaFormatter.git
    cd LuaFormatter
    cmake .
    make
    make install
```

## Usage
`lua-format -c {your_style_file} {your_lua_file}`

or use default style:

`lua-format {your_lua_file}`

### Style configure file

See [Wiki Page](https://github.com/Koihik/LuaFormatter/wiki/Style-Config)

## Limitations

* Do not work when source file contains syntax error
* Do not support 'Format selection'
