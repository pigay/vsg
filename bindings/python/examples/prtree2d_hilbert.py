import sys
import vsg

HK2_0_1 = 0
HK2_0_2 = 1
HK2_3_1 = 2
HK2_3_2 = 3

s = {
    HK2_0_1 : [0, 2, 3, 1],
    HK2_0_2 : [0, 1, 3, 2],
    HK2_3_1 : [3, 2, 0, 1],
    HK2_3_2 : [3, 1, 0, 2],
}

d = {
    HK2_0_1 : [HK2_0_2, HK2_0_1, HK2_0_1, HK2_3_1],
    HK2_0_2 : [HK2_0_1, HK2_0_2, HK2_0_2, HK2_3_2],
    HK2_3_1 : [HK2_3_2, HK2_3_1, HK2_3_1, HK2_0_1],
    HK2_3_2 : [HK2_3_1, HK2_3_2, HK2_3_2, HK2_0_2],
}

# example with bound method
class Toto:
    def hilbert2_order (self, node_key, children, children_keys):
        #print node_key, children, children_keys, 
        for i in range (4):
            children[i] = s[node_key][i]
            children_keys[i] = d[node_key][i]
        #print ",", node_key, children, children_keys

def write (pt, f):
    pt.write (f)
    f.write ("\n")

pts = []

lvl = 1

try:
    lvl = int (sys.argv[1])
except:
    pass

n = 1 << lvl
np = n*n

for i in range (n):
    x = 2. * (i+0.5) / n - 1.
    for j in range (n):
        y = 2. * (j+0.5) / n - 1.

        pts.append (vsg.Vector2d (x, y))

lbound = vsg.Vector2d (-1., -1.)
ubound = vsg.Vector2d (1., 1.)

prtree = vsg.PRTree2d (lbound, ubound,
                       vsg.Vector2d.vector2d_locfunc,
                       vsg.Vector2d.dist,
                       None, 1)

for pt in pts:
    #print pt
    prtree.insert_point (pt)

t = Toto ()

prtree.set_children_order (t.hilbert2_order, HK2_0_1)

prtree.foreach_point (write, sys.stdout)

prtree.set_children_order (None, None)

