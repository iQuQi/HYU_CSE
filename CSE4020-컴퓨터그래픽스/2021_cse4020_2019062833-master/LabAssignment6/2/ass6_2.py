import glfw
import numpy as np
from OpenGL.GL import *
from OpenGL.GLU import *
import math
gVertexArrayIndexed=None
gIndexArray=None
ang_a=45
ang_b=30
rad=5

def render():
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)
    #이전에 그린게 위로 오도록
    #이거 안해주면 나중에 그린게 덮어
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)
    glLoadIdentity()

    gluPerspective(45,1,1,10)
    cam_y=rad*np.sin(np.radians(ang_b))
    orth_rad=rad*np.cos(np.radians(ang_b))
    cam_x=orth_rad*np.sin(np.radians(ang_a))
    cam_z=orth_rad*np.cos(np.radians(ang_a))

    at_x=0
    at_y=0
    at_z=0

    up_x=0
    up_y=1
    up_z=0

    gluLookAt(cam_x,cam_y,cam_z,at_x,at_y,at_z,up_x,up_y,up_z)

    drawFrame()
    glColor3ub(255,255,255)
    drawCube()
        
def drawCube():
    global gVertexArrayIndexed, gIndexArray
    varr=gVertexArrayIndexed
    iarr=gIndexArray
    glEnableClientState(GL_VERTEX_ARRAY)
    glVertexPointer(3,GL_FLOAT,3*varr.itemsize,varr)
    glDrawElements(GL_QUADS,iarr.size, GL_UNSIGNED_INT,iarr)
    
def createvertexAndIndexArrayIndexed():
    varr=np.array([
        (0,1.5,1.5),
        (0,1.5,0),
        (1.5,1.5,0),
        (1.5,1.5,1.5),
        (0,0,1.5),
        (0,0,0),
        (1.5,0,0),
        (1.5,0,1.5),
        ],'float32')
    iarr=np.array([
        (0, 3,2,1),
        (0,4,7,3),
        (3,7,6,2),
        (2,6,5,1),
        (0,1,5,4),
        (4,7,6,5),

        ])

    return varr,iarr

def keyboard(window, key, scancode, action, mods):
            global ang_a,ang_b
            if action==glfw.PRESS or action==glfw.REPEAT:
                if key==glfw.KEY_1:
                  ang_a-=10
                elif key==glfw.KEY_3:
                  ang_a+=10
                elif key==glfw.KEY_2:
                  ang_b+=10
                elif key==glfw.KEY_W:
                  ang_b-=10

    
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




def main():
    global gVertexArrayIndexed, gIndexArray
    if not glfw.init(): return
    window=glfw.create_window(480,480,"2019062833",None,None)
    if not window:
        glfw.terminate()
        return

    glfw.make_context_current(window)
    gVertexArrayIndexed,gIndexArray = createvertexAndIndexArrayIndexed()
    glfw.set_key_callback(window, keyboard)
    while not glfw.window_should_close(window):
        glfw.poll_events()
        render()
        glfw.swap_buffers(window)


    glfw.terminate()

if __name__ == "__main__":
    main()

