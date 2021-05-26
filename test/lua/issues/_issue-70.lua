t:func()[1].key = true

local function foo()
    t:func()[1].key = true
    t:func()[1].key = true
end

local function bar() t:func()[1].key = true end
