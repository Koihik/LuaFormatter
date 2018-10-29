# LuaFormatter

Reformats your Lua source code.

[![Build Status](https://travis-ci.org/Koihik/LuaFormatter.svg?branch=master)](https://travis-ci.org/Koihik/LuaFormatter)
[![codecov](https://codecov.io/gh/Koihik/LuaFormatter/branch/master/graph/badge.svg)](https://codecov.io/gh/Koihik/LuaFormatter)

## Usage
`lua-format your_lua_file.lua`

## Feature

### indent
before:
```lua
function a() local b = function() doSomething(); end
```

after:
```lua
function a()
    local b = function()
        doSomething()
    end
end
```

### style
before:
```lua
tbl = {
    key="value";
    key2=function() print("function value") end;
    
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
    end,

    -- custom new line
    key3 = pcall(function()
        return "ww"
    end)
}

```

## Compiling
Build from source.

### Requirements
* c++ 11 compiler
* cmake
* antlr4-cpp-runtime 

### Steps
```bash
    brew install antlr4-cpp-runtime
    cmake .
    make
```