function foo()
  x = 1
  y = 2
end

function bar( x, y ) local f = function( r, s ) print( 'hello' ) end end

function baz( tab ) bar( tab.x, tab.y ) end

print( 1, 2, 2 )

baz{ x=3, y=4 }

z = { x=2, y=4 }

y = { z=2, x=4 }

foo()
