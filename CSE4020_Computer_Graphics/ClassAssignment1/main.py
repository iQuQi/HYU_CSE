import glfw
import numpy as np
from OpenGL.GLU import *
from OpenGL.GL import *

up_down=0
projection = 0
cam_x=0
cam_y=0
cam_z=0

camAng_a=45
camAng_b=30
rad=3

target_x=0
target_y=0
target_z=0

up_x=0
up_y=1
up_z=0

bias_x = 0
bias_y = 0
bias_z = 0

#True False 할때 앞에 대문자임
mouse_press_left = False
mouse_press_right = False

beforePos=[0,0]
def render():
    global zoom,cam_x,cam_y,cam_z, up_down , up_y,up_z,up_x, target_x,target_y,target_z, bias_x, bias_y, bias_z
    global camAng_a,camAng_b ,rad,projection
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)
    #이전에 그린게 위로 오도록,
    #이거 안해주면 나중에 그린게 덮어
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)
    glLoadIdentity()

    #둘의 zNear zFar 는 같게 
    if projection ==0:
        gluPerspective(45,1,1,50)
    elif projection ==1:
        glOrtho(-5,5,-5,5,1,50)

    #라디안으로 바꿔주는 거 잊지말자
    if up_down==0:
        cam_y=rad*np.sin(np.radians(camAng_b))
        orth_rad=rad*np.cos(np.radians(camAng_b))
        cam_x=orth_rad*np.sin(np.radians(camAng_a))
        cam_z=orth_rad*np.cos(np.radians(camAng_a))
        up_y=1
        
    else :
        cam_y=rad*np.sin(np.radians(camAng_b))
        orth_rad=rad*np.cos(np.radians(camAng_b))
        cam_x=orth_rad*np.sin(np.radians(camAng_a+180))
        cam_z=orth_rad*np.cos(np.radians(camAng_a+180))
        up_y= -1
        

    gluLookAt(cam_x-bias_x,cam_y-bias_y,cam_z-bias_z,target_x-bias_x,target_y-bias_y,target_z-bias_z,up_x,up_y,up_z)

    drawFrame()
    glColor3ub(255,255,255)


def drawLineRow(x):
    if x==0:glColor3ub(255,0 ,0)
    glVertex3fv(np.array([-20,0,x]))
    glVertex3fv(np.array([20,0,x]))
    glColor3ub(255,255 ,255)

def drawLineCol(x):
    if x==0:glColor3ub(0,255 ,0)
    glVertex3fv(np.array([x,0,-20]))
    glVertex3fv(np.array([x,0,20]))
    glColor3ub(255,255 ,255)

    
def drawFrame():
    glBegin(GL_LINES)
    glColor3ub(255,255 ,255)
    for i in range(-100,100,5):
        drawLineRow(i/5)
        
    glColor3ub(0,0,255)
    glVertex3fv(np.array([0,-10,0]))
    glVertex3fv(np.array([0,10,0]))
    
    glColor3ub(255,255 ,255)
    for i in range(-100,100,5):
        drawLineCol(i/5)
    glEnd()




def mouse_button_callback( window, button, action,mods):
    global  mouse_press_left, mouse_press_right
    if button == glfw.MOUSE_BUTTON_LEFT:
        if(action == glfw.PRESS or action==glfw.REPEAT) : mouse_press_left=True
        elif action==glfw.RELEASE :  mouse_press_left=False
       
        
    elif button == glfw.MOUSE_BUTTON_RIGHT:
        if(action == glfw.PRESS or action==glfw.REPEAT): mouse_press_right=True
        elif action==glfw.RELEASE :  mouse_press_right=False
        
          
     

def scroll_callback( window,  xoffset, yoffset):
    global rad
    if yoffset == 1.0 and rad>1 :
        #줌인
        rad-=1
       
    elif yoffset==-1.0:
        #줌아웃
        rad+=1


def key_callback(window, key, scancode, action, mods):
        global projection
        if action==glfw.PRESS:
            if key==glfw.KEY_V:
                projection=(projection+1)%2


def cursor_pos_callback(window, xpos,ypos):
    global beforePos, mouse_press_left,mouse_press_right, bias_x, bias_y, bias_z,rad,up_down
    global camAng_a,camAng_b,up_x,up_y,up_z,cam_x,cam_y,cam_z, target_x,target_y,target_z
    
    dx=(beforePos[0]-xpos)/5
    dy=(beforePos[1]-ypos)/5
    
    if mouse_press_left==True :
        camAng_b=(camAng_b - dy)%(360)
        if camAng_b>90 and camAng_b<270:
            camAng_a=(camAng_a - dx)%(360)
            up_down=1
        else :
            camAng_a=(camAng_a + dx)%(360)
            up_down=0
        
    if mouse_press_right==True :
        w_x=(cam_x-target_x)
        w_y=(cam_y-target_y)
        w_z=(cam_z-target_z)
        
        w=np.array([w_x,w_y,w_z])
        up=np.array([up_x,up_y,up_z])

        #cross가 고등학교때 배운 그 외적 하는법
        #이걸 쓸려면 인자로 들어가는 array가 np.array로 만들어져있어야함. list x
        u=np.cross(up,w)/(np.dot(np.cross(up,w),np.cross(up,w)))**1/2
        v=np.cross(w,u)/(np.dot(np.cross(w,u),np.cross(w,u)))**1/2
       
        
        bias_x+=(u[0]*(-dx)+v[0]*(dy/rad))*rad**2/170
        bias_y+=(u[1]*(-dx)+v[1]*(dy/rad))*rad**2/170
        bias_z+=(u[2]*(-dx)+v[2]*(dy/rad))*rad**2/170
        

    beforePos[0]=xpos
    beforePos[1]=ypos
    
def main():
    if not glfw.init(): return
    window=glfw.create_window(1200,800,"cg_classAssignment",None,None)
    if not window:
        glfw.terminate()
        return

    glfw.set_cursor_pos_callback(window, cursor_pos_callback)
    glfw.set_mouse_button_callback (window, mouse_button_callback);
    glfw.set_key_callback (window, key_callback);
    glfw.set_scroll_callback (window, scroll_callback);
    glfw.make_context_current(window)
    
  
    while not glfw.window_should_close(window):
        glfw.poll_events()
        render()
        glfw.swap_buffers(window)


    glfw.terminate()

if __name__ == "__main__":
    main()
