from sys import stderr, stdout

import vsg

v1 = vsg.Vector3f (0., 0., 0.)

v1.x = 1.

v2 = vsg.Vector3f (0., 1., 0.)

print v1*v2

(v1^v2).write (stdout)
stdout.write ("\n")
v3 = v1 + v2

print v3.x, v3.y, v3.z, "",
v3.write (stdout)
stdout.write ("\n")

v3 = v3 * 3.

print v3.x, v3.y, v3.z, "",
v3.write (stdout)
stdout.write ("\n")

v3 = v3 / 3.5

print v3.x, v3.y, v3.z, "",
v3.write (stdout)
stdout.write ("\n")

v3[0] = 13.
v3[1] = 34.
v3[2] = 45.

for i in range (len (v3)):
    print v3[i],
print

