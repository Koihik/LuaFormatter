function func1()
    -- do nothing
    return 1; -- delete
end
    
x, y = 1, func1; -- keep
(function() print("123");  end)()

local x, y = 1, func1; -- keep
(function() print("123");  end)()

local x, y = 1, func1; -- delete
print("123")

print(1, 2, 3); -- keep
(function() print("123");  end)(); -- delete
print("123")


repeat 
    -- empty
until y; -- keep
(function() print("123");  end)()
repeat
    -- empty
until y; -- delete
repeat 
    -- empty
until x == y; -- keep
(function() print("123");  end)()
