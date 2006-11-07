#! /usr/bin/env python
######################################################################
# Python example of using LibVsg in OpenGL/GLUT visualisation.
######################################################################

from math import *
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import vsg


######################################################################
# abstract class for visible objects
######################################################################
class GLRenderer (object):
    def gl_render (self):
        raise NotImplementedError
    def get_bounds (self):
        raise NotImplementedError

######################################################################
# Class mouse interaction (zoom, pan) within a 2D display.
######################################################################
class GL2DNav (object):
    def __init__ (self, x, y, w, h, center = vsg.V2D_ZERO, scale = 1.,
                  ratio = 0.):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.center = center
        self.scale = scale
        if ratio <= 0.: ratio = float (w)/h
        self.ratio = ratio

        self.__old_x = 0
        self.__old_y = 0
        self.__pressed_button = -1

        self.right_button_cb = None
        self.ctrl_right_button_cb = None

    def show_box (self, lb, ub):
        self.center = (lb+ub) * 0.5
        scale = ub-self.center
        self.scale = min (scale.x, scale.y)
        glutPostRedisplay ()

    def motion (self, x, y):
        if self.__pressed_button < 0: return
        dx = (x - self.__old_x) * 1. / self.w
        dy = (self.__old_y - y) * 1. / self.h
        if self.__pressed_button == GLUT_LEFT_BUTTON:
            # pan
            dx *= 2. * self.scale
            dy *= 2. * self.scale
            pan = vsg.Vector2d (dx, dy)
            self.center = self.center - pan
        elif self.__pressed_button == GLUT_MIDDLE_BUTTON:
            # zoom
            self.scale /= (1.+0.9*dy)

        self.__old_x = x
        self.__old_y = y

        glutPostRedisplay ()

    def mouse (self, button, state, x, y):
        if (state == GLUT_DOWN):
            self.__pressed_button = button
            self.__old_x = x
            self.__old_y = y
        else:
            if button == GLUT_RIGHT_BUTTON:
                x1 = 2. * float (x)/self.w - 1.
                y1 = 1. - float (y)/self.h * 2.
                xx = (self.center.x + x1*self.scale) * self.ratio
                yy = self.center.y + y1*self.scale
                modifiers = glutGetModifiers ()
                if modifiers & GLUT_ACTIVE_CTRL:
                    self.ctrl_right_button_cb (xx, yy, 3*self.scale/self.h)
                else:
                    self.right_button_cb (xx, yy)
            self.__pressed_button = -1
            self.__old_x = 0
            self.__old_y = 0

    def prepare (self):
        # Projection
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        
        lb = self.center - self.scale * vsg.Vector2d (1., 1.)
        ub = self.center + self.scale * vsg.Vector2d (1., 1.)
        gluOrtho2D (lb.x*self.ratio, ub.x*self.ratio, lb.y, ub.y)

        # Initialize ModelView matrix
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

######################################################################
# Class for displaying 2D objects with GLUT.
######################################################################
class GLUTWindow (object):
    def __init__ (self, title, x=0, y=0, w=640, h=640):
        self.title = title
        self.x = x
        self.y = y
        self.w = w
        self.h = h

        self.renderers = {}

        self.__id = self.__realize ()
        glutDisplayFunc (self.draw)
        glutKeyboardFunc (self.key)

        self.nav = GL2DNav (self.x, self.y, self.w, self.h)
        glutMouseFunc (self.nav.mouse)
        glutMotionFunc (self.nav.motion)

    def destroy (self):
        glutDestroyWindow (self.__id)
        self.__id = -1

    def __realize (self):
        glutInitWindowPosition (self.x, self.y)
        glutInitWindowSize (self.w, self.h)
        return glutCreateWindow (self.title)

    def show_box (self, lb, ub):
        self.nav.show_box (lb, ub)

    def register (self, name, renderer):
        lb, ub = renderer.get_bounds ()
        self.show_box (lb, ub)
        self.renderers[name] = renderer

    def key (self, c, x, y):
        self.destroy ()

    def draw (self):
        self.prepare ()
        for renderer in self.renderers.values ():
            renderer.gl_render ()
        glutSwapBuffers ()

    def prepare (self):
        # Viewport
        glViewport(0, 0, self.w, self.h)

        # Initialize
        glClearColor(0., 0., 0., 0.)
        glClear(GL_COLOR_BUFFER_BIT);

        self.nav.prepare ()

######################################################################
######################################################################

if __name__ == "__main__":
    # Initialize Glut
    glutInit ([])
    glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE)


    # Open a window
    win = GLUTWindow ("vis", 0, 0, 640, 480)

    glutMainLoop ()
