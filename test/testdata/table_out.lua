q = {
    a = "1",
    b = 1,

    -- line break
    c = function() return 1 end,
    d = {
        [1] = {},
        [2] = {},
        [xyz] = (function()
            return {
                1,
                2,
                3
            }
        end)()
    }
}
