-- varDecl
x, y = 1, 2;

-- label
::LBL_A::

-- whileStat gotoStat
while 1 == 2 do goto LBL_A end

-- doStat
do print("something"); end

-- repeatStat
repeat x = 2 + 3 or 1 + 2 until y == 2

-- forStat
for i = 1, 5, 2 do print(i) end

for i = 1, 5 do print(i) end

-- forInStat
a = {"1", "2", "3"}
for i, v in ipairs(a) do print(i, v) end

-- localVarDecl
local q, w, e = 1, 2, 3
local p, o;

-- ifStat
if x then
    print(x)
elseif y == 2 then
    -- empty
else
    -- do nothing
end
