#! /usr/local/bin/lua

func = function()
    return function()
        print("inner");
        print((function() return "qqq" .. "www"; end)())
    end
    --[[end]]
end
