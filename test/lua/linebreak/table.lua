-- simple table
x = {}
y = {1,2,3}

-- table contains line comment
y = {1,2-- line comment
}

-- table too long
x = {
    1,2,long_namespace.long_object.long_method:long_method(1,2,3),
    long_namespace.long_object.long_method:long_method(1,2,3),
}

-- table contains kv
y = {
    1,2,
    [k] = 'v',
}

-- table contains something have line break
x = {
    function()
        print(1)
        print(2)
    end
}

-- empty table
x = {--empty
}

x = {1222222, 2222222222, 22223, 222224, 52222, 62, 7, 8, 9, 11111, 11111, 1111}

x = {1222222, 2222222222, 22223, 222224, 522222, 62, 7, 8, 9, 11111, 11111, 1111}

x = {1222222, 2222222222, 22223, 222224, 522222, 622, 7, 8, 9, 11111, 11111, 1111}

x = {1222222, 2222222222, 22223, 222224, 522222, 6222, 7, 8, 9, 11111, 11111,1111}
