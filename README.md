# LuaFormatter

Reformats your Lua source code.

[![Build Status (Linux & MacOS)](https://travis-ci.org/Koihik/LuaFormatter.svg?branch=master)](https://travis-ci.org/Koihik/LuaFormatter)
[![Build status (Windows)](https://ci.appveyor.com/api/projects/status/to7uvpkdgj96aumg/branch/master?svg=true)](https://ci.appveyor.com/project/Koihik/luaformatter/branch/master)
[![codecov](https://codecov.io/gh/Koihik/LuaFormatter/branch/master/graph/badge.svg)](https://codecov.io/gh/Koihik/LuaFormatter)

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
`lua-format -c your_style_file your_lua_file.lua`

or use default style:

`lua-format your_lua_file.lua`

### Style configure file
```yml
indent: '    '

# table separator, ',' or ';'
table_sep: ','

# add a extra separator at the end of table
extra_sep_at_table_end: false

# chop down parameter if characters of parameter >= 50 (without white space)
chop_down_parameter: 50

# chop down block if characters of block >= 50 (without white space)
chop_down_block: 50

# chop down table if characters of table >= 50 (without white space)
chop_down_table: 50

# keep simple block one line
keep_simple_block_one_line: true
```

## Feature

### indent
before:
```lua
function a() local b = function() doSomething() doSomething2() end end
```

after:
```lua
function a()
    local b = function()
        doSomething()
        doSomething2()
    end
end

```

### style
before:
```lua
tbl = {
    key="value";
    key2=function() print("function value"); print("long function body"); end;
    
    -- custom blank line
    key3=pcall(function() return "ww";end)
}
```

after:
```lua
tbl = {
    key = "value",
    key2 = function()
        print("function value")
        print("long function body")
    end,

    -- custom blank line
    key3 = pcall(function() return "ww" end)
}

```

## Limitations

* Do not work when source file contains syntax error
* Do not support 'Format selection'
