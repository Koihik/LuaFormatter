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

The easiest way to install is to use [LuaRocks](https://github.com/luarocks/luarocks).

```bash
luarocks install --server=https://luarocks.org/dev luaformatter
```

### Build from source

#### Requirements
* cmake 3.9+
* c++ 17 compiler
* luarocks 3.3.0+

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

The program will attempt to automatically use the current directory's `.lua-format` file if no config file is passed in the command line.
In case there's no file, it will fallback to the default configuration.

### Style configure file

Configuration parameters not specified fallback to their default values.

See [this file](https://github.com/Koihik/LuaFormatter/blob/master/docs/Style-Config.md)

### Default configuration

```
column_limit: 80
indent_width: 4
use_tab: false
tab_width: 4
continuation_indent_width: 4
spaces_before_call: 1
keep_simple_control_block_one_line: true
keep_simple_function_one_line: true
align_args: true
break_after_functioncall_lp: false
break_before_functioncall_rp: false
align_parameter: true
chop_down_parameter: false
break_after_functiondef_lp: false
break_before_functiondef_rp: false
align_table_field: true
break_after_table_lb: true
break_before_table_rb: true
chop_down_table: false
chop_down_kv_table: true
table_sep: ","
extra_sep_at_table_end: false
break_after_operator: true
double_quote_to_single_quote: false
single_quote_to_double_quote: false
```
## Limitations

* Do not work when source file contains syntax error
* Do not support 'Format selection'
