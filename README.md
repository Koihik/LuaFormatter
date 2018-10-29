# LuaFormatter

Reformats your Lua source code.

[![Build Status](https://travis-ci.org/Koihik/LuaFormatter.svg?branch=master)](https://travis-ci.org/Koihik/LuaFormatter)
[![codecov](https://codecov.io/gh/Koihik/LuaFormatter/branch/master/graph/badge.svg)](https://codecov.io/gh/Koihik/LuaFormatter)

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

# allow format simple function to one-line
keep_simple_function_one_line: true

# allow format simple table to one-line
keep_simple_table_one_line: true
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
    
    -- custom new line
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

    -- custom new line
    key3 = pcall(function() return "ww" end)
}

```

## Compiling
Build from source, only support mac os now (Linux should also work).

### Requirements
* c++ 11 compiler
* cmake 3.0+
* antlr4-cpp-runtime 4.7.1+

### Steps
```bash
    brew install antlr4-cpp-runtime
    cmake .
    make
```