import glfw
import numpy as np
from OpenGL.GL import *
import math
gComposedM =np.identity(3)


def render(th):
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

    #th를 사용해서 m1 m2를 계산
    R=np.array([[np.cos(th),-np.sin(th),0],
                 [np.sin(th),np.cos(th),0],
                   [0,0,1]])
    T1=np.array([[1,0,0],
                 [0,1,0.5],
                   [0,0,1]])
    T2=np.array([[1,0,0.5],
                 [0,1,0],
                   [0,0,1]])

    
    M1=R@T1
    M2=R@T2
    

    #p 포인트그리기
    glBegin(GL_POINTS)
    glVertex2fv((M1@np.array([0.,0.5,1]))[:-1])
    
    glVertex2fv((M2@np.array([0.5,0,1]))[:-1])
    glEnd()
    

    #v 벡터 그리기
    glBegin(GL_LINES)
    glVertex2fv((M1@np.array([0,0,0]))[:-1])
    glVertex2fv((M1@np.array([0.,0.5,0]))[:-1])

    glVertex2fv((M2@np.array([0,0,0]))[:-1])
    glVertex2fv((M2@np.array([0.5,0,0]))[:-1])
    glEnd()

    
def main():
    if not glfw.init(): return
    window=glfw.create_window(480,480,"2019062833",None,None)
    if not window:
        glfw.terminate()
        return

    glfw.make_context_current(window)
    
  
    while not glfw.window_should_close(window):
        glfw.poll_events()
        t=glfw.get_time()
        render(-t)
        glfw.swap_buffers(window)


    glfw.terminate()

if __name__ == "__main__":
    main()

