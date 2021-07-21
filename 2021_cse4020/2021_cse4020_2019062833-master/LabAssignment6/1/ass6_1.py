import glfw
import numpy as np
from OpenGL.GL import *
from OpenGL.GLU import *
import math

def render():
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)
    glLoadIdentity()

    #glFrustum(-1,1,-1,1,1,10)
    #gluLookAt(5,3,5,1,1,-1,0,1,0)
    myFrustum(-1,1,-1,1,1,10)
    myLookAt(np.array([5,3,5]),np.array([1,1,-1]),np.array([0,1,0]))
    
    drawFrame()
    glColor3ub(255,255,255)
    drawCubeArray()


def myFrustum(left, right, bottom,top, near,far):
   
    l=left
    r=right
    b=bottom
    t=top
    n=near
    f=far
    
    M_pj=np.array([[2*n/(r-l),0,(r+l)/(r-l),0],
                    [0,2*n/(t-b),(t+b)/(t-b),0],
                    [0,0,-(f+n)/(f-n),-2*f*n/(f-n)],
                    [0,0,-1,0]])
    #multi 아니고 mult 임!
    glMultMatrixf(M_pj.T)
                  

def myLookAt(eye,at,up):

      
      w=(eye-at)/np.sqrt((np.dot((eye-at),(eye-at))))
      u=np.cross(up,w)/np.sqrt((np.dot(np.cross(up,w),np.cross(up,w))))
      v=np.cross(w,u)/np.sqrt((np.dot(np.cross(w,u),np.cross(w,u))))

     
      M_v=np.array([[u[0],u[1],u[2],-np.dot(u,eye)],
                    [v[0],v[1],v[2],-np.dot(v,eye)],
                    [w[0],w[1],w[2],-np.dot(w,eye)],
                    [0,0,0,1]])

      glMultMatrixf(M_v.T)

    

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

