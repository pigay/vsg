from sys import stderr, stdout

import vsg

v1 = vsg.Vector2f (0., 0.)

v1.x = 1.
v1.write (stdout)
stdout.write ("\n")

v2 = vsg.Vector2f (0., 1.)
v2.write (stdout)
stdout.write ("\n")

print v1^v2, v1*v2

v3 = v1 + v2

print v3.x, v3.y, "",
v3.write (stdout)
stdout.write ("\n")

v3 = v3 * 2.

print v3.x, v3.y, "",
v3.write (stdout)
stdout.write ("\n")

v3 = v3 / 3.5

print v3.x, v3.y, "",
v3.write (stdout)
stdout.write ("\n")

v3[0] = 12.
v3[1] = 24.

for i in range (len (v3)):
    print v3[i],
print


print v3, v3.to_polar ()
print v3, v3.to_polar_internal ()
