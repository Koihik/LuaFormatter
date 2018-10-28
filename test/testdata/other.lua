
local funcb1 = function(a,b,c) print("func b1"); end;
local funcb2 = function(a,b,c,...) print("func b2"); end
local funcb3 = function(--[[what?]]...) print("func b3"); end
local funcb = function() 
print("func b");
local bb = function() print("bb");local bbb = function() end end
 end

func = function() 
return function() print("inner");print((function() return "qqq".."www"; end)()) end
--[[end]]end

funca()
funcb()
print("---");
func();
local x = func;
(function() print("func c");end)--[[www]]()

print(call("23")--[[wwwe]])

local f = f or function()
    print("f")
end

xxx:qqq():www():eee({
    func= function() print("xx") end,
    func1 = function() print("www") end;
    [ab_c] = {
        "sss"; "qqq"
    }
})

do 
print("1")
--[[end]]end

function xx()
if c1 then
    f1()
elseif c2 then
    --do nothing
elseif c3 then
    f3()
else
    --comment1
    --[[comment2]]fx()
    --comment3
end
end

--last comment