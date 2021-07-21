import glfw
import numpy as np
from OpenGL.GL import *
import math
key_list = []

def render():
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

    glColor3ub(255, 255, 255)
    
    for key in key_list:
        if(key=='q'):
            glTranslatef(-0.1,0,0)
        elif(key=='e'):
            glTranslatef(0.1,0,0)
        elif(key=='a'):
            glRotatef(10,0,0,1)
        elif (key=='d'):
            glRotatef(-10,0,0,1)
            
    drawTriangle()

def drawTriangle():    
    # draw triangle
    glBegin(GL_TRIANGLES)
    glVertex2fv( (np.array([.0,.5])))
    glVertex2fv( (np.array([.0,.0])))
    glVertex2fv( (np.array([.5,.0])))
    glEnd()

def keyboard(window, key, scancode, action, mods):
        global key_list
        if action==glfw.PRESS or action==glfw.REPEAT:
            if key==glfw.KEY_Q:
                key_list.insert(0,"q")
            elif key==glfw.KEY_E:
                key_list.insert(0,"e")
            elif key==glfw.KEY_A:
                key_list.insert(0,"a")
            elif key==glfw.KEY_D:
                key_list.insert(0,"d")
            elif (key-48)==1:
                key_list=[]
                glLoadIdentity()

                

    
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
        render()
        glfw.swap_buffers(window)


    glfw.terminate()

if __name__ == "__main__":
    main()

