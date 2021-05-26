function test()
    local x = {
        [qqq] = {aaa, bbb, ccc},
        -- LuaFormatter off
        [www] = {
            aaa,
            bbb,
            ccc
        },
        -- LuaFormatter on
        [eee] = {aaa, bbb, ccc},
    }
end
