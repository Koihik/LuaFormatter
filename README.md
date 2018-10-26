# LuaFormatter

Format your Lua code.
Pending...

## Requirements
* antlr4-runtime v4.7.1
* googletest v1.8.1

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
call(a,b,c,{
    1,2,3;4;5
})
```

after:
```lua
call(a, b, c, {
    1,
    2,
    3,
    4,
    5,
})
```
