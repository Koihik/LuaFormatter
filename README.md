# LuaFormatter

Reformats your Lua source code. Try online [lua formatter](https://goonlinetools.com/lua-beautifier/)

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

      -h, --help                             Display this help menu
      -v, --verbose                          Turn on verbose mode
      -i                                     Reformats in-place
      --dump-config                          Dumps the default style used to stdout
      -c[file], --config=[file]              Style config file
      Lua scripts...                         Lua scripts to format
      --column-limit=[column limit]          Column limit of one line
      --indent-width=[indentation
      width]                                 Number of spaces used for indentation
      --tab-width=[tab width]                Number of spaces used per tab
      --continuation-indent-width=[Continuation
      indentation width]                     Indent width for continuations line
      --spaces-before-call=[spaces
      before call]                           Space on function calls
      --column-table-limit=[column
      table limit]                           Column limit of each line of a table
      --table-sep=[table separator]          Character to separate table fields
      This group is all exclusive:
        --use-tab                            Use tab for indentation
        --no-use-tab                         Do not use tab for indentation
      This group is all exclusive:
        --keep-simple-control-block-one-line
                                             keep block in one line
        --no-keep-simple-control-block-one-line
                                             Do not keep block in one line
      This group is all exclusive:
        --keep-simple-function-one-line      keep function in one line
        --no-keep-simple-function-one-line
                                             Do not keep function in one line
      This group is all exclusive:
        --align-args                         Align the arguments
        --no-align-args                      Do not align the arguments
      This group is all exclusive:
        --break-after-functioncall-lp        Break after '(' of function call
        --no-break-after-functioncall-lp     Do not break after '(' of function call
      This group is all exclusive:
        --break-before-functioncall-rp       Break before ')' of function call
        --no-break-before-functioncall-rp    Do not break before ')' of function call
      This group is all exclusive:
        --align-parameter                    Align the parameters
        --no-align-parameter                 Do not align the parameters
      This group is all exclusive:
        --chop-down-parameter                Chop down all parameters
        --no-chop-down-parameter             Do not chop down all parameters
      This group is all exclusive:
        --break-after-functiondef-lp         Break after '(' of function def
        --no-break-after-functiondef-lp      Do not break after '(' of function def
      This group is all exclusive:
        --break-before-functiondef-rp        Break before ')' of function def
        --no-break-before-functiondef-rp     Do not break before ')' of function def
      This group is all exclusive:
        --align-table-field                  Align fields of table
        --no-align-table-field               Do not align fields of table
      This group is all exclusive:
        --break-after-table-lb               Break after '{' of table
        --no-break-after-table-lb            Do not break after '{' of table
      This group is all exclusive:
        --break-before-table-rb              Break before '}' of table
        --no-break-before-table-rb           Do not break before '}' of table
      This group is all exclusive:
        --chop-down-table                    Chop down any table
        --no-chop-down-table                 Do not chop down any table
      This group is all exclusive:
        --chop-down-kv-table                 Chop down table if table contains key
        --no-chop-down-kv-table              Do not chop down table if table contains key
      This group is all exclusive:
        --extra-sep-at-table-end             Add a extra field separator
        --no-extra-sep-at-table-end          Do not add a extra field separator
      This group is all exclusive:
        --break-after-operator               Put break after operators
        --no-break-after-operator            Do not put break after operators
      This group is all exclusive:
        --double-quote-to-single-quote       Transform string literals to use single quote
        --no-double-quote-to-single-quote    Do not transform string literals to use single quote
      This group is all exclusive:
        --single-quote-to-double-quote       Transform string literals to use double quote
        --no-single-quote-to-double-quote    Do not transform string literals to use double quote
      "--" can be used to terminate flag options and force all following
      arguments to be treated as positional options
```

The program will attempt to automatically use the current directory's `.lua-format` file if no config file is passed in the command line. If none is found, it will try to locate a `.lua-format` file in a parent directory recursively. On Linux it will use `$XDG_CONFIG_HOME/luaformatter/config.yaml` if `.lua-format` does not exist.
In case there's no file, it will fallback to the default configuration.
The program will give the top priority to the configuration values given in the command-line, then to the configuration files and finally to the hard-coded default values.

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
column_table_limit: column_limit
extra_sep_at_table_end: false
break_after_operator: true
double_quote_to_single_quote: false
single_quote_to_double_quote: false
```
## Limitations

* Do not work when source file contains syntax error
* Do not support 'Format selection'
