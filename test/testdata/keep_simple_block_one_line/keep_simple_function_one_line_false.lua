function x()
    print("hello")
end

function x() print("hello") end

local function x()
    print("hello")
end

local function x() print("hello") end

x = function()
    print("hello")
end

x = function() print("hello") end

if true then
    print("hello")
end

if true then
    print("hello")
elseif true then
    print("hello")
end

if true then print("hello") end

while true do
    print("hello")
end

while true do print("hello") end

repeat
    print("hello")
until true

repeat print("hello") until true

do
    print("hello")
end

do print("hello") end

for _ = 1, 10, 1 do
    print("hello")
end

for _ = 1, 10, 1 do print("hello") end

for _ in pairs({}) do
    print("hello")
end

for _ in pairs({}) do print("hello") end
