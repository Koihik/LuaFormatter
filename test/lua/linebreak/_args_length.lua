for i = 3, 5, 2 do
    for j = 1, 6 do
        assert((debug.upvalueid(a[i], 4) == debug.upvalueid(a[j], 4)) ==
                   (i == j))
    end
end
