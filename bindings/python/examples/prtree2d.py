import vsg

from glutwindow import *

class Circle (object):
    def __init__ (self, center, radius):
        self.center = center
        self.radius = radius

    def loc2 (circle, center):
        tmp = center - circle.center

        dist = tmp.norm ()

        if dist <= circle.radius: return vsg.RLOC2_MASK

        retult = 0
        center_pos = circle.center.vector2d_locfunc (center)

        result |= vsg.RLOC2_COMP (center_pos)

        if fabs(tmp.x) <= circle.radius:
            itmp = center_pos & ~ (vsg.LOC2_X & center_pos)
            result |= vsg.RLOC2_COMP (itmp)

        if fabs(tmp.y) <= circle.radius:
            itmp = center_pos & ~ (vsg.LOC2_Y & center_pos)
            result |= vsg.RLOC2_COMP (itmp)

        return result

def _vertex2d (vector):
    glVertex2d (vector.x, vector.y)

def _display_node (node_info, user_data):
    lbound = node_info.lbound
    ubound = node_info.ubound

    se = vsg.Vector2d (ubound.x, lbound.y)
    nw = vsg.Vector2d (lbound.x, ubound.y)

    _vertex2d (lbound)
    _vertex2d (se)

    _vertex2d (se)
    _vertex2d (ubound)

    _vertex2d (ubound)
    _vertex2d (nw)

    _vertex2d (nw)
    _vertex2d (lbound)

def _display_point (point, user_data):
    _vertex2d (point)

class PRTree2dRenderer (GLRenderer):
    def __init__ (self):
        lbound = vsg.Vector2d (-.5, -.5)
        ubound = vsg.Vector2d (.5, .5)
        self.prtree = vsg.PRTree2d (lbound, ubound,
                                    vsg.Vector2d.vector2d_locfunc,
                                    vsg.Vector2d.dist,
                                    Circle.loc2, 1)

    def gl_render (self):
        glColor3f (0., 1., 0.)

        glBegin (GL_LINES)
        self.prtree.traverse (vsg.G_PRE_ORDER, _display_node, None)
        glEnd ()

        glColor3f (1., 1., 1.)
        glBegin (GL_POINTS)
        self.prtree.foreach_point (_display_point, None)
        glEnd ()

    def get_bounds (self):
        lbound = vsg.V2D_ZERO.clone ()
        ubound = vsg.V2D_ZERO.clone ()
        self.prtree.get_bounds (lbound, ubound)
        return lbound, ubound

    def add_point (self, x, y):
        point = vsg.Vector2d (x, y)
        self.prtree.insert_point (point)
        glutPostRedisplay ()
        print "add", x, y

if __name__ == "__main__":
    # Initialize Glut
    glutInit ([])
    glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE)


    # Open a window
    win = GLUTWindow ("prtree2d", 0, 0, 640, 480)

    ptree2drender = PRTree2dRenderer ()
    win.register ("ptree2drender", ptree2drender)
    win.nav.right_button_cb = ptree2drender.add_point
    glutMainLoop ()
