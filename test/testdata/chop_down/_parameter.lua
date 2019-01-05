-- parameter
call(1, 2, 3, 4)

-- parameter contains line comment
call(
    1,
    2, -- line comment
    3,
    4
)

-- parameter too long
call(
    {123, 32},
    long_namespace.long_object.long_method:long_method(1, 2, 3)
)
