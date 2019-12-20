function t() return function() end end
local a = t{1, 2, 3, 4}
local a = t{1}
local a = t"a"
local a = t"b"
local a = t(1, 2, 3)
t{2, 3} "a"
t{2, 3, 4} {1}
