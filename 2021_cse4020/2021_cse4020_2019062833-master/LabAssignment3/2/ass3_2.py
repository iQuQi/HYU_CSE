import glfw
import numpy as np
from OpenGL.GL import *
import math
gComposedM =np.identity(3)

def keyboard(window, key, scancode, action, mods):
        global gComposedM
        if action==glfw.PRESS or action==glfw.REPEAT:
            if key==glfw.KEY_D:
                D=np.array([[np.cos(np.deg2rad(-10)),-np.sin(np.deg2rad(-10)),0],
                           [np.sin(np.deg2rad(-10)),np.cos(np.deg2rad(-10)),0],
                           [0,0,1]])
                gComposedM=D@gComposedM
            elif key==glfw.KEY_W:
                W=np.array([[1,0,0],
                           [0,0.9,0],
                           [0,0,1]])
                gComposedM=W@gComposedM
            elif key==glfw.KEY_E:
                E=np.array([[1,0,0],
                           [0,1.1,0],
                           [0,0,1]])
                gComposedM=E@gComposedM
            elif key==glfw.KEY_S:
                S=np.array([[np.cos(np.deg2rad(10)),-np.sin(np.deg2rad(10)),0],
                           [np.sin(np.deg2rad(10)),np.cos(np.deg2rad(10)),0],
                           [0,0,1]])
                gComposedM=S@gComposedM
            elif key==glfw.KEY_X:
                X=np.array([[1,0,0.1],
                           [0,1,0],
                           [0,0,1]])
                gComposedM=X@gComposedM
            elif key==glfw.KEY_C:
                C=np.array([[1,0,-0.1],
                           [0,1,0],
                           [0,0,1]])
                gComposedM=C@gComposedM
            elif key==glfw.KEY_R:
                R=np.array([[-1,0,0],
                           [0,-1,0],
                           [0,0,1]])
                gComposedM=R@gComposedM
            elif (key-48)==1:
                gComposedM =np.identity(3)

                            

def render(T):
    glClear(GL_COLOR_BUFFER_BIT)
    glLoadIdentity()
    
    # draw cooridnate
    glBegin(GL_LINES)
    glColor3ub(255, 0, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([1.,0.]))
    glColor3ub(0, 255, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([0.,1.]))
    glEnd()
    
    # draw triangle
    glBegin(GL_TRIANGLES)
    glColor3ub(255, 255, 255)
    glVertex2fv( (T @ np.array([.0,.5,1.]))[:-1] )
    glVertex2fv( (T @ np.array([.0,.0,1.]))[:-1] )
    glVertex2fv( (T @ np.array([.5,.0,1.]))[:-1] )
    glEnd()

    
def main():
    if not glfw.init(): return
    window=glfw.create_window(480,480,"2019062833",None,None)
    if not window:
        glfw.terminate()
        return

    glfw.set_key_callback(window, keyboard)
    glfw.make_context_current(window) 
  
    while not glfw.window_should_close(window):
        glfw.poll_events()
        render(gComposedM)
        glfw.swap_buffers(window)


    glfw.terminate()

if __name__ == "__main__":
    main()

