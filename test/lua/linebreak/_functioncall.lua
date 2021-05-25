-- long chained method call
ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss.w(wwwwwwwww, ddq,
                                                               q.w(qqqqqq,
                                                                   wsssssssw),
                                                               q.w(qqqqqq,
                                                                   wsssssssw),
                                                               q.w(qqqqqq,
                                                                   wsssssssw),
                                                               q.w(qqqqqq,
                                                                   wsssssssw))

local x = function()
    ddd.www:map(function(it)
        it.aaa(wwww)(wwww)(wwww)(wwww) -- comment
        .wwwww(wwww)(wwww)(wwww)(wwww).wwwww(wwww)(wwww)(wwww)(wwww)
            .wwwww(wwww)(wwww)(wwww)(wwww).wwwww(wwww):wwwww(wwww):wwwww(wwww)
            :wwwww(wwww):wwwww(wwww):wwwww(wwww)
    end)
end

call(1, 2, 3, 4)

-- args contains line comment
call(1, 2, -- line comment
3, 4)

call({123, 32},
     long_namespace.long_object.long_method:long_method():long_method(1, 2, 3))

-- nested explist
call(1, 2, 3, {
    x = function()
        print(1)
        print "2"

        local q, w, e = a, b, c
    end
}, function() return 1, 2, 3 end)

x(1, 111111, 111111, 1111111, 1111111, 11111111, 1111111111, 11111111, 1111111)

x(11, 111111, 111111, 1111111, 1111111, 11111111, 1111111111, 11111111, 1111111)

x(111, 111111, 111111, 1111111, 1111111, 11111111, 1111111111, 11111111, 1111111)

x(1111, 111111, 111111, 1111111, 1111111, 11111111, 1111111111, 11111111,
  1111111)

x(1111111111, 111111, 111111, 1111111, 1111111, 11111111, 1111111111, 11111111,
  1)
