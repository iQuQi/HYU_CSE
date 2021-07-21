import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
import time
from OpenGL.arrays import vbo

gCamAng = 0.
gCamHeight = 1.
frame = 0
def slerp(R1,R2,t):
    return R1 @ exp(t*log(R1.T @ R2))

def exp(rv):
    ang= np.sqrt(rv[0]**2 + rv[1]**2 + rv[2]**2)
    u = rv/ang
    ang_c = np.cos(ang)
    ang_s = np.sin(ang)
    R = np.array([[ang_c+u[0]**2*(1-ang_c),
                   u[0]*u[1]*(1-ang_c)-u[2]*ang_s,
                   u[0]*u[2]*(1-ang_c)+u[1]*ang_s]
                  
                 ,[u[1]*u[0]*(1-ang_c)+u[2]*ang_s,
                   ang_c+u[1]**2*(1-ang_c),
                   u[1]*u[2]*(1-ang_c)-u[0]*ang_s]
                  
                 ,[u[2]*u[0]*(1-ang_c)-u[1]*ang_s,
                   u[2]*u[1]*(1-ang_c)+u[0]*ang_s,
                   ang_c+u[2]**2*(1-ang_c)]])
    
    return R

def log(R):
    if((R[0,0]+R[1,1]+R[2,2])==3):
        ang = 0
        v = 0
    elif((R[0,0]+R[1,1]+R[2,2])==-1):
        ang = np.pi
        v = np.array([R[0,2],R[1,2],1+R[2,2]])/(np.sqrt(2*(1+R[2,2])))*ang
        if(v==0):
            v = np.array([R[0,1],1+R[1,1],R[2,1]])/(np.sqrt(2*(1+R[1,1])))*ang
            if(v==0): v = np.array([1+R[0,0],R[1,0],R[2,0]])/(np.sqrt(2*(1+R[0,0])))*ang
    else:
        ang = np.arccos((R[0,0]+R[1,1]+R[2,2]-1)/2.0)
        v= np.array([(R[2,1]-R[1,2])/(2*np.sin(ang)) , (R[0,2]-R[2,0])/(2*np.sin(ang)), (R[1,0]-R[0,1])/(2*np.sin(ang))])*ang
       
    return v

def createVertexAndIndexArrayIndexed():
    varr = np.array([
            ( -0.5773502691896258 , 0.5773502691896258 ,  0.5773502691896258 ),
            ( -1 ,  1 ,  1 ), # v0
            ( 0.8164965809277261 , 0.4082482904638631 ,  0.4082482904638631 ),
            (  1 ,  1 ,  1 ), # v1
            ( 0.4082482904638631 , -0.4082482904638631 ,  0.8164965809277261 ),
            (  1 , -1 ,  1 ), # v2
            ( -0.4082482904638631 , -0.8164965809277261 ,  0.4082482904638631 ),
            ( -1 , -1 ,  1 ), # v3
            ( -0.4082482904638631 , 0.4082482904638631 , -0.8164965809277261 ),
            ( -1 ,  1 , -1 ), # v4
            ( 0.4082482904638631 , 0.8164965809277261 , -0.4082482904638631 ),
            (  1 ,  1 , -1 ), # v5
            ( 0.5773502691896258 , -0.5773502691896258 , -0.5773502691896258 ),
            (  1 , -1 , -1 ), # v6
            ( -0.8164965809277261 , -0.4082482904638631 , -0.4082482904638631 ),
            ( -1 , -1 , -1 ), # v7
            ], 'float32')
    iarr = np.array([
            (0,2,1),
            (0,3,2),
            (4,5,6),
            (4,6,7),
            (0,1,5),
            (0,5,4),
            (3,6,2),
            (3,7,6),
            (1,2,6),
            (1,6,5),
            (0,7,3),
            (0,4,7),
            ])
    return varr, iarr

def drawCube_glDrawElements():
    global gVertexArrayIndexed, gIndexArray
    varr = gVertexArrayIndexed
    iarr = gIndexArray
    glEnableClientState(GL_VERTEX_ARRAY)
    glEnableClientState(GL_NORMAL_ARRAY)
    glNormalPointer(GL_FLOAT, 6*varr.itemsize, varr)
    glVertexPointer(3, GL_FLOAT, 6*varr.itemsize, ctypes.c_void_p(varr.ctypes.data + 3*varr.itemsize))
    glDrawElements(GL_TRIANGLES, iarr.size, GL_UNSIGNED_INT, iarr)

def drawFrame():
    glBegin(GL_LINES)
    glColor3ub(255, 0, 0)
    glVertex3fv(np.array([0.,0.,0.]))
    glVertex3fv(np.array([3.,0.,0.]))
    glColor3ub(0, 255, 0)
    glVertex3fv(np.array([0.,0.,0.]))
    glVertex3fv(np.array([0.,3.,0.]))
    glColor3ub(0, 0, 255)
    glVertex3fv(np.array([0.,0.,0]))
    glVertex3fv(np.array([0.,0.,3.]))
    glEnd()

def xyz_euler(x,y,z):
    x=np.radians(x)
    y=np.radians(y)
    z=np.radians(z)
    Rx = np.array([[1,0,0],
                 [0,np.cos(x),-np.sin(x)],
                 [0,np.sin(x),np.cos(x)]])
    Ry = np.array([[np.cos(y),0,np.sin(y)],
                 [0,1,0],
                 [-np.sin(y),0,np.cos(y)]])
    
    Rz = np.array([[np.cos(z),-np.sin(z),0],
                 [np.sin(z),np.cos(z),0],
                 [0,0,1]])

    return Rx @ Ry @ Rz

def render_frame(r,g,b,R1_euler,R2_euler):
        objectColor = (r,g,b,1.)
        specularObjectColor = (1.,1.,1.,1.)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, objectColor)
        glMaterialfv(GL_FRONT, GL_SHININESS, 10)
        glMaterialfv(GL_FRONT, GL_SPECULAR, specularObjectColor)
        
        R1 = np.identity(4)
        R1[:3,:3] = R1_euler
        J1 = R1
        
        glPushMatrix()
        glMultMatrixf(J1.T)
        glPushMatrix()
        glTranslatef(0.5,0,0)
        glScalef(0.5, 0.05, 0.05)
        drawCube_glDrawElements()
        glPopMatrix()
        glPopMatrix()

        R2 = np.identity(4)
        R2[:3,:3] =  R2_euler
        T1 = np.identity(4)
        T1[0][3] = 1.

        J2 = R1 @ T1 @ R2

        glPushMatrix()
        glMultMatrixf(J2.T)
        glPushMatrix()
        glTranslatef(0.5,0,0)
        glScalef(0.5, 0.05, 0.05)
        drawCube_glDrawElements()
        glPopMatrix()
        glPopMatrix()


        return R1[:3,:3],R2[:3,:3]
    

def render(t):
    
    global gCamAng, gCamHeight,frame
    
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)

    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1, 1,10)

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    gluLookAt(5*np.sin(gCamAng),gCamHeight,5*np.cos(gCamAng), 0,0,0, 0,1,0)

    # draw global frame
    drawFrame()

    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)

    glEnable(GL_RESCALE_NORMAL)

    lightPos = (3.,4.,5.,1.)
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos)

    lightColor = (1.,1.,1.,1.)
    ambientLightColor = (.1,.1,.1,1.)
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor)
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor)
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightColor)

    R1_euler =xyz_euler(20,30,30)
    R2_euler =xyz_euler(15,30,25)
    R1_0,R2_0 = render_frame(1,0,0,R1_euler,R2_euler)

    R1_euler =xyz_euler(45,60,40)
    R2_euler =xyz_euler(25,40,40)
    R1_1,R2_1 = render_frame(1,1,0,R1_euler,R2_euler)

    R1_euler =xyz_euler(60,70,50)
    R2_euler =xyz_euler(40,60,50)
    R1_2,R2_2 = render_frame(0,1,0,R1_euler,R2_euler)

    R1_euler =xyz_euler(80,85,70)
    R2_euler =xyz_euler(55,80,65)
    R1_3,R2_3 = render_frame(0,0,1,R1_euler,R2_euler)

    if frame == 0 :
        render_frame(1,0,0,slerp(R1_0,R1_1,(frame)/20),slerp(R2_0,R2_1,(frame)/20))
    elif 0<frame and frame<20: render_frame(1,1,1,slerp(R1_0,R1_1,(frame)/20),slerp(R2_0,R2_1,(frame)/20))

    elif frame == 20 :
        render_frame(1,1,0,slerp(R1_1,R1_2,(frame)/20-1),slerp(R2_1,R2_2,(frame)/20-1))
    elif 20<frame and frame<40:render_frame(1,1,1,slerp(R1_1,R1_2,(frame)/20-1),slerp(R2_1,R2_2,(frame)/20-1))
    
    elif frame == 40 :
        render_frame(0,1,0,slerp(R1_2,R1_3,(frame)/20-2),slerp(R2_2,R2_3,(frame)/20-2))
    elif 40<frame and frame<60:render_frame(1,1,1,slerp(R1_2,R1_3,(frame)/20-2),slerp(R2_2,R2_3,(frame)/20-2))
    
    elif frame == 60 :
        render_frame(0,0,1,slerp(R1_2,R1_3,(frame)/20-2),slerp(R2_2,R2_3,(frame)/20-2))

    glDisable(GL_LIGHTING)
    


def key_callback(window, key, scancode, action, mods):
    global gCamAng, gCamHeight
    # rotate the camera when 1 or 3 key is pressed or repeated
    if action==glfw.PRESS or action==glfw.REPEAT:
        if key==glfw.KEY_1:
            gCamAng += np.radians(-10)
        elif key==glfw.KEY_3:
            gCamAng += np.radians(10)
        elif key==glfw.KEY_2:
            gCamHeight += .1
        elif key==glfw.KEY_W:
            gCamHeight += -.1

gVertexArrayIndexed = None
gIndexArray = None

def main():
    global gVertexArrayIndexed, gIndexArray,frame
    if not glfw.init():
        return
    window = glfw.create_window(640,640,'2019062833', None,None)
    if not window:
        glfw.terminate()
        return
    glfw.make_context_current(window)
    glfw.set_key_callback(window, key_callback)
    glfw.swap_interval(1)

    gVertexArrayIndexed, gIndexArray = createVertexAndIndexArrayIndexed()

    while not glfw.window_should_close(window):
        glfw.poll_events()
        
        t = (3*glfw.get_time()) % 1
        if frame == 62 :
            frame = 0
        render(t)
        frame +=1
        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()

