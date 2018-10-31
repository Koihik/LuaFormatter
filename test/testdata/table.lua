key = "some key"

q = {
    a="1",
    b=1;

    -- line break
    c=function() return 1 end,
    d={
        [1] = {};
        [2] = {};
        [key] = (function() return {1,2,3} end)()
    }
}

q[ [=[key1]=] ] = "value1"
q[ [[key2]] ] = "value2"
q['\'"key3"'] = "value3"
q["\"key4\""] = "value4"