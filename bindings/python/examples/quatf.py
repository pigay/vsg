from sys import stderr, stdout

import vsg

q1 = vsg.Quaternionf (0., 0., 0., 1.)

q3 = q1 * 2.

print q3.x, q3.y, q3.z, q3.w, "",
q3.write (stdout)
stdout.write ("\n")

q3 = q3 * 3.

print q3.x, q3.y, q3.z, q3.w, "",
q3.write (stdout)
stdout.write ("\n")

q3 = q3 / 3.5

print q3.x, q3.y, q3.z, q3.w, "",
q3.write (stdout)
stdout.write ("\n")

q3[0] = 13.
q3[1] = 34.
q3[2] = 45.
q3[3] = 56.

for i in range (len (q3)):
    print q3[i],
print

