import sys
import vsg
import copy

class Circle:
    def __init__ (self, center, radius):
        self.center = center
        self.radius = radius

    def loc2 (self, center):
        tmp = center - self.center

        dist = tmp.norm ()

        if dist <= self.radius: return vsg.RLOC2_MASK

        center_loc = vsg.Vector2d.vector2d_locfunc (self.center, center)

        center_rloc = vsg.RLOC2_COMP (center_loc)
        result = center_rloc

        if (abs (tmp.x) <= self.radius):
            itmp = (center_loc & ~ vsg.LOC2_X) | (~ center_loc & vsg.LOC2_X)
            result |= vsg.RLOC2_COMP (itmp)

        if (abs (tmp.y) <= self.radius):
            itmp = (center_loc & ~ vsg.LOC2_Y) | (~ center_loc & vsg.LOC2_Y)
            result |= vsg.RLOC2_COMP (itmp)

        return result

    def write (self, file):
        file.write ("Circle {c=")
        self.center.write (file)
        file.write (", r=%f}\n" % self.radius)

def vector2d_write (vector, file):
    vector.write (file)
    file.write ("\n")

p0=vsg.Vector2d (0., 0.)
p1=vsg.Vector2d (1., 1.)

t=vsg.PRTree2d (p0, p1, vsg.Vector2d.vector2d_locfunc, vsg.Vector2d.dist,
                 Circle.loc2, 1)

print vsg.Vector2d.vector2d_locfunc (p1, p0)

pts = [
    vsg.Vector2d (-1., -1.), vsg.Vector2d (0., -1.),
    vsg.Vector2d (-1., 0.), vsg.Vector2d (-0.4, -0.4)
    ]
print pts

for p in pts:
    t.insert_point (p)

p2=vsg.Vector2d (0., 0.)
p3=vsg.Vector2d (1., 1.)
t.get_bounds (p2, p3)
print p2, p3

for p in pts:
    t.remove_point (p)

t.get_bounds (p2, p3)
print p2, p3

for p in pts:
    t.insert_point (p)

t.foreach_point (vector2d_write, sys.stderr)

c = Circle (vsg.Vector2d (-0.4, -0.4), .5)

t.insert_region (c)
t.write (sys.stderr)
t.foreach_region (Circle.write, sys.stderr)

t.remove_region (c)

t.write (sys.stderr)

v = t.find_point (vsg.Vector2d (0.5, 0.5))
print v
v = t.find_point (pts[0])
print v

try:
    def foo ():
        pass
    t.foreach_point (foo, None)
    sys.stderr.write ("foreach_point error uncaught\n")
except TypeError:
    sys.stderr.write ("foreach_point error correctly handled\n")

sys.stderr.write ("foreach_point_custom:\n")
t.foreach_point_custom (c, Circle.loc2, vector2d_write, sys.stderr)

sys.stderr.write ("foreach_region_custom:\n")
t.insert_region (Circle (vsg.Vector2d (.1, 1), .05))
t.insert_region (c)
t.foreach_region_custom (c, Circle.write, sys.stderr)

sys.stderr.write ("find_deep_region:\n")
s = t.find_deep_region (vsg.Vector2d (0.5, 0.5), None, None)
if s: s.write (sys.stderr)
else: sys.stderr.write ("%s\n" % `s`)

sys.stderr.write ("traverse:\n")
def _traverse_func (node_info, data):
    data.write ("[lbound= ")
    node_info.lbound.write (data)
    data.write (", ubound= ")
    node_info.ubound.write (data)
    data.write (", point= ")
    data.write (`node_info.point_list`)
    data.write (", region= ")
    data.write (`node_info.region_list`)
    data.write (", user_data= ")
    data.write (`node_info.user_data`)
    data.write (", father_info= ")
    data.write (`node_info.father_info`)
    data.write (", isleaf= ")
    data.write (`node_info.isleaf`)
    data.write ("]\n")

t.traverse (vsg.G_PRE_ORDER, _traverse_func, sys.stderr)

sys.stderr.write ("traverse_custom:\n")
t.traverse_custom (vsg.G_PRE_ORDER, c, _traverse_func, sys.stderr)

sys.stderr.write ("near_far_traversal:\n")
def _near_func (one_info, other_info, data):
    data.write ("near interaction [onecenter= ")
    one_info.center.write (data)
    data.write (", othercenter= ")
    other_info.center.write (data)
    data.write ("]\n")

def _far_func (one_info, other_info, data):
    data.write ("far interaction [onecenter= ")
    one_info.center.write (data)
    data.write (", othercenter= ")
    other_info.center.write (data)
    data.write ("]\n")

t.insert_point (vsg.Vector2d (.1, .2))
t.insert_point (vsg.Vector2d (.2, .1))
t.insert_point (vsg.Vector2d (.2, .2))

t.near_far_traversal (_far_func, _near_func, sys.stderr)

sys.stderr.write ("set_node_data:\n")
class NodeData:
    def __init__ (self, msg):
        self.msg = msg
    def copy (self):
        return copy.copy (self)
    def __repr__ (self):
        return "NodeData "+self.msg

t.set_node_data (NodeData ('a'))
t.traverse (vsg.G_PRE_ORDER, _traverse_func, sys.stderr)
t.set_node_data (NodeData ('b'))
t.traverse (vsg.G_PRE_ORDER, _traverse_func, sys.stderr)
#
try:
    t.insert_point (1)
except ValueError:
    sys.stderr.write ("insert_point invalid argument exception caught\n")
try:
    t.insert_region (1)
except ValueError:
    sys.stderr.write ("insert_region invalid argument exception caught\n")

try:
    t.foreach_point_custom ("z", Circle.loc2, vector2d_write, sys.stderr)
except ValueError:
    sys.stderr.write ("foreach_point_custom invalid argument exception caught\n")

t.set_node_data (None)

sys.stderr.write ("t.clone ()\n")
t2 = t.clone ()

sys.stderr.write ("del t\n")
del t

sys.stderr.write ("set_node_data t2\n")
t2.set_node_data (NodeData ("c"))

sys.stderr.write ("del t2\n")
del t2

sys.stderr.write ("ok\n")
