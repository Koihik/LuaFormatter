-- simple table
x = {}
y = {1, 2, 3}

-- table contains line comment
y = {
    1, 2 -- line comment
}

-- table too long
x = {
    1, 2, long_namespace.long_object.long_method:long_method(1, 2, 3),
    long_namespace.long_object.long_method:long_method(1, 2, 3)
}

-- table contains kv
y = {1, 2, [k] = 'v'}

-- table contains something have line break
x = {
    function()
        print(1)
        print(2)
    end
}

-- empty table
x = { -- empty
}
