-- keep
if x then print(1) end

-- chop down
if x then
    print(1)
    print(2)
end

-- chop down
if x then
    print(1)
else
    print(2)
end

-- keep
while x do y() end

-- keep
while x do for a in b do y() end end

-- too long, chop down
while x do
    for i in k do local sumFunction = function(a, b) return a + b end end
end

-- keep
repeat x() until y
