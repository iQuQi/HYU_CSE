import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
from OpenGL.arrays import vbo

gCamAng = 0.
gCamHeight = 1.
before_end = np.array([0,0,0,1])
now_end = np.array([0,0,0,1])


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

def draw_endFrame():
    glBegin(GL_LINES)
    glColor3ub(255, 255, 255)
    glVertex3fv(np.array([0.5,0.,0]))
    glVertex3fv(np.array([1.0,0.,0]))

    glVertex3fv(np.array([0.5,0.,0]))
    glVertex3fv(np.array([0.5,0.5,0]))
   
    glVertex3fv(np.array([0.5,0.,0]))
    glVertex3fv(np.array([0.5,0.,0.5]))
    glEnd()

def velocity_line():
    global before_end,now_end
    tmp_v =now_end - before_end
    next_end = now_end + 10*tmp_v

   
    glBegin(GL_LINES)
    glColor3ub(255, 0, 0)
    glVertex3fv(now_end[:3])
    glVertex3fv(next_end[:3])
    glEnd()
        
    

def render(t):
    global gCamAng, gCamHeight,before_end,now_end
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

    objectColor = (1.,1.,1.,1.)
    specularObjectColor = (1.,1.,1.,1.)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, objectColor)
    glMaterialfv(GL_FRONT, GL_SHININESS, 10)
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularObjectColor)
    
    R1 = np.identity(4)
    #y축 방향으로 t rad 회전 . XYZ 오일러
    
    R1_x = np.array([[1,0,0],
                 [0,np.cos(0),-np.sin(0)],
                 [0,np.sin(0),np.cos(0)]])
    R1_y = np.array([[np.cos(t),0,np.sin(t)],
                 [0,1,0],
                 [-np.sin(t),0,np.cos(t)]])
    R1_z = np.array([[np.cos(0),-np.sin(0),0],
                 [np.sin(0),np.cos(0),0],
                 [0,0,1]])
    R1[:3,:3] = R1_x @ R1_y @ R1_z
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
    
    
    R2_x = np.array([[1,0,0],
                 [0,np.cos(t),-np.sin(t)],
                 [0,np.sin(t),np.cos(t)]])
    R2_y = np.array([[np.cos(0),0,np.sin(0)],
                 [0,1,0],
                 [-np.sin(0),0,np.cos(0)]])
    R2_z = np.array([[np.cos(t),-np.sin(t),0],
                 [np.sin(t),np.cos(t),0],
                 [0,0,1]])

    R2[:3,:3] =  R2_x @ R2_y @ R2_z
    



    
    T1 = np.identity(4)
    T1[0][3] = 1.

    J2 = R1 @ T1 @ R2

    glPushMatrix()
    glMultMatrixf(J2.T)
    glPushMatrix()
    glTranslatef(0.5,0,0)
    draw_endFrame()
   
    glScalef(0.5, 0.05, 0.05)
    drawCube_glDrawElements()
    
    glPopMatrix()
    glPopMatrix()
    
    glDisable(GL_LIGHTING)
    now_end = R1 @ T1 @ R2 @ np.array([1,0,0,1]).T
    velocity_line()
    before_end = now_end


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
    global gVertexArrayIndexed, gIndexArray
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
        
        t = glfw.get_time()
        render(t)

        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()

