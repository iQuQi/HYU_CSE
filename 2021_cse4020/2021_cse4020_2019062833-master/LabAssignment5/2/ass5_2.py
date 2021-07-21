import glfw
import numpy as np
from OpenGL.GL import *
from OpenGL.GLU import *
import math
gComposedM =np.identity(3)


def render():
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)
    #이전에 그린게 위로 오도록
    #이거 안해주면 나중에 그린게 덮어
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)
    glLoadIdentity()

    gluPerspective(45,1,1,10)

    glRotatef(35.264,1,0,0)
    glRotatef(-45,0,1,0)
    glTranslatef(-3,-3,-3)
    
   
  
    
    drawFrame()
    glColor3ub(255,255,255)
    drawCubeArray()

    
def drawFrame():
    glBegin(GL_LINES)
    glColor3ub(255,0 ,0)
    glVertex3fv(np.array([0,0,0]))
    glVertex3fv(np.array([1,0,0]))
    glColor3ub(0,255 ,0)
    glVertex3fv(np.array([0,0,0]))
    glVertex3fv(np.array([0,1,0]))
    glColor3ub(0,0,255)
    glVertex3fv(np.array([0,0,0]))
    glVertex3fv(np.array([0,0,1]))
    glEnd()


def drawUnitCube():
    glBegin(GL_QUADS)
    glVertex3f(0.5, 0.5, -0.5)
    glVertex3f(-0.5 ,0.5, -0.5)
    glVertex3f(-0.5, 0.5, 0.5)
    glVertex3f(0.5 ,0.5, 0.5)

    glVertex3f(0.5, -0.5, 0.5)
    glVertex3f(-0.5 ,-0.5, 0.5)
    glVertex3f(-0.5, -0.5, -0.5)
    glVertex3f(0.5 ,-0.5, -0.5)

    glVertex3f(0.5, 0.5, 0.5)
    glVertex3f(-0.5 ,0.5, 0.5)
    glVertex3f(-0.5, -0.5, 0.5)
    glVertex3f(0.5 ,-0.5, 0.5)

    glVertex3f(0.5, -0.5,-0.5)
    glVertex3f(-0.5 ,-0.5, -0.5)
    glVertex3f(-0.5, 0.5, -0.5)
    glVertex3f(0.5 ,0.5, -0.5)

    glVertex3f(-0.5, 0.5, 0.5)
    glVertex3f(-0.5 ,0.5, -0.5)
    glVertex3f(-0.5, -0.5, -0.5)
    glVertex3f(-0.5 ,-0.5, 0.5)

    glVertex3f(0.5, 0.5, -0.5)
    glVertex3f(0.5 ,0.5, 0.5)
    glVertex3f(0.5, -0.5, 0.5)
    glVertex3f(0.5 ,-0.5,-0.5)
    glEnd()
    
def drawCubeArray():
    for i in range(5):
        for j in range(5):
            for k in range(5):
                glPushMatrix()
                glTranslate(i,j,-k-1)
                glScalef(.5,.5,.5)
                drawUnitCube()
                glPopMatrix()



def main():
    if not glfw.init(): return
    window=glfw.create_window(480,480,"2019062833",None,None)
    if not window:
        glfw.terminate()
        return

    glfw.make_context_current(window)
    
  
    while not glfw.window_should_close(window):
        glfw.poll_events()
        render()
        glfw.swap_buffers(window)


    glfw.terminate()

if __name__ == "__main__":
    main()

