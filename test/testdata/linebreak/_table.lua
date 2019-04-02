-- table
x = {}
y = {1}

-- table contains line comment
y = { -- c1
    1, -- c2
    2 -- c3
}

-- table too long
x = {
    1, 2, long_namespace.long_object.long_method:long_method(1, 2, 3),
    [long_long_key] = 'long_long_value', [long_long_key] = 'long_long_value',
    [long_long_key] = 'long_long_value'
}
y = {1, 2, [long_long_key] = 'long_long_value'}

-- table contains something have line break
x = {
    [key] = function()
        print(1)
        print(2)
    end
}
y = {[key] = function() print(1) end}