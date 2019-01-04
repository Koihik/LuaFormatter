function f1() print("f1") end
local function f2() print("f2") end

function ellipsis(...) print("ellipsis") end
function arg_ellipsis(a, b, c, d, ...) print("arg_ellipsis") end

function long(q, w, e)
    x = q + w + e
    x = x * 2
    return {x, 1, 2}
end
