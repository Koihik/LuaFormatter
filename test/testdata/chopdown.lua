-- parameter
call(1, 2, 3, 4)

-- parameter contains line comment
call(1, 2, -- line comment
3, 4)

-- parameter too long
call({123,32},long_namespace.long_object.long_method:long_method(1,2,3))

-- table
x = {}
y = {1}

-- table contains line comment
y = {1,2-- line comment
}

-- table too long
x = {
    1,2,long_namespace.long_object.long_method:long_method(1,2,3),
    [long_long_key] = 'long_long_value'
}
y = {
    1,2,
    [long_long_key] = 'long_long_value',
}

-- table contains something have line break
x = {
    [key] = function()
        print(1)
        print(2)
    end
}
y = {
    [key] = function()
        print(1)
    end
}
