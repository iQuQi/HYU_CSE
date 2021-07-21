import glfw
import numpy as np
from OpenGL.GLU import *
from OpenGL.GL import *
from OpenGL.arrays import vbo
import ctypes
gVertexPosArray = np.empty((0,3), float) # 해당 평면에서의 vertex 마다의 위치
gVertexNorArray = np.empty((0,3), float) # 해당 평면에서의 vertex 마다의 normal

gFaceIndexArray = np.empty((0,3), float)
gVertexAvgArray = [] # 각 vertex의 평균 nomral값
#gVertexAvgCntArray = [] # 각 vertex에 겹치는 vertex 수


gPosIndexArray = [] # vertex 위치 리스트
gNorIndexArray = [] # normal 벡터 리스트
storeMesh = []

animate_mode =0
frame_mode=1
force_shading_mode=0

face_cnt = 0
ver_3_face = 0
ver_4_face =0
ver_more_face =0
file_name = ""

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

mouse_press_left = False
mouse_press_right = False

beforePos=[0,0]
def render():
    global zoom,cam_x,cam_y,cam_z, up_down , up_y,up_z,up_x, target_x,target_y,target_z, bias_x, bias_y, bias_z
    global camAng_a,camAng_b ,rad,projection, frame_mode, force_shading_modem, animate_mode
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)
    
    glMatrixMode(GL_PROJECTION)
    if frame_mode ==1 :
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)
    else :
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
        
    glLoadIdentity()
    if projection ==0:
        gluPerspective(45,1,1,50)
    elif projection ==1:
        glOrtho(-5,5,-5,5,1,50)

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    
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

    #lighting code
    glEnable(GL_LIGHTING)  
    glEnable(GL_LIGHT0)
    glEnable(GL_LIGHT1)
    glEnable(GL_NORMALIZE)

    # light position
    glPushMatrix()
    lightPos_0 = (16.,18.,20.,0)   #directional light 
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos_0)
    lightPos_1 = (-3.,4.,5.,0)    #directional light
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos_1)
    glPopMatrix()

    

    # light intensity for each color channel
    lightColor = (1,1,1,1.)
    ambientLightColor = (.1,.1,.1,1.)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor)
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor)
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightColor)

    lightColor = (0.3,0,0,1.)
    ambientLightColor = (.1,.1,.1,1.)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor)
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightColor)
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLightColor)

    # material reflectance for each color channel
    objectColor = (1.,1.,1.,1.)
    specularObjectColor = (1.,1.,1.,1.)
    glMaterialfv(GL_FRONT, GL_SHININESS, 15)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, objectColor)
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularObjectColor)

    glPushMatrix()
    #animate mode
    if animate_mode ==1 :
        t_1 = glfw.get_time()
        t_2 = (np.cos(glfw.get_time()) + 1)/8
        
        
        #cloud transformation
        glPushMatrix()
        glTranslatef(2*np.cos(t_1), 2*np.sin(t_1), 0)
        glTranslatef(9,13,-27)
        
        #cloud drawing
        glPushMatrix()
        glScalef(0.1,0.1,0.1)
        objectColor = (0.6,0.8,1.,1.)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, objectColor)
        if force_shading_mode == 1 : draw_glDrawElements_animate(0)
        else : draw_glDrawArrays_animate(0)
        glPopMatrix()

        #baby transformation
        glPushMatrix()
        glTranslatef(0,0.7,0)
        glTranslatef(-9,-13,27)
        glRotatef(np.sin(t_1)*30,1,0,0)

        #baby drawing
        glPushMatrix()
        glScalef(0.03,0.03,0.03)
        glRotatef(-90,1,0,0)
        objectColor = (1.,.8,.7,1.)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, objectColor)
        if force_shading_mode == 1 : draw_glDrawElements_animate(1)
        else : draw_glDrawArrays_animate(1)
        glPopMatrix()

        #wings transformation
        glPushMatrix()
        glTranslatef(0, 0, -t_2)
        glTranslatef(0,0,-0.5)

        #wings drawing
        glPushMatrix()
        glScalef(0.001,0.001,0.001)
        objectColor = (1,1,1,1.)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, objectColor)
        if force_shading_mode == 1 : draw_glDrawElements_animate(2)
        else : draw_glDrawArrays_animate(2)
        glPopMatrix()

        glPopMatrix()
        glPopMatrix()
        glPopMatrix()


    #single mesh mode 
    else :
        if force_shading_mode == 1: draw_glDrawElements()
        else : draw_glDrawArrays()
        
      
        
    glPopMatrix()
    glDisable(GL_LIGHTING)

def draw_glDrawElements_animate(i):
    global storeMesh
    varr = np.array(storeMesh[i][2],'float32') #gPosIndexArray
    narr = np.array(storeMesh[i][4],'float32') #gVertexAvgArray
    iarr = storeMesh[i][5]                     #gFaceIndexArray
    
    glEnableClientState(GL_VERTEX_ARRAY)
    glEnableClientState(GL_NORMAL_ARRAY)
    
    glNormalPointer(GL_FLOAT,3*narr.itemsize,narr)
    glVertexPointer(3, GL_FLOAT, 3*varr.itemsize, varr)
    glDrawElements(GL_TRIANGLES,iarr.size,GL_UNSIGNED_INT,iarr)

def draw_glDrawArrays_animate(i):
    global storeMesh
    varr = storeMesh[i][0] #gVertexPosArray
    narr = storeMesh[i][1] #gVertexNorArray
    
    glEnableClientState(GL_VERTEX_ARRAY)
    glEnableClientState(GL_NORMAL_ARRAY)
    
    glNormalPointer(GL_FLOAT,3*narr.itemsize,narr)
    glVertexPointer(3, GL_FLOAT, 3*varr.itemsize, varr)
    glDrawArrays(GL_TRIANGLES,0,int(varr.size/3)) 


def draw_glDrawArrays():
    global gVertexPosArray,gVertexNorArray
    varr = gVertexPosArray
    narr = gVertexNorArray
    
    glEnableClientState(GL_VERTEX_ARRAY)
    glEnableClientState(GL_NORMAL_ARRAY)
    
    glNormalPointer(GL_FLOAT,3*narr.itemsize,narr)
    glVertexPointer(3, GL_FLOAT, 3*varr.itemsize, varr)
    glDrawArrays(GL_TRIANGLES,0,int(varr.size/3))


def draw_glDrawElements():
    global gVertexAvgArray,gPosIndexArray,gFaceIndexArray
    varr = np.array(gPosIndexArray,'float32')
    narr = np.array(gVertexAvgArray,'float32')
    iarr = gFaceIndexArray
   
    
    glEnableClientState(GL_VERTEX_ARRAY)
    glEnableClientState(GL_NORMAL_ARRAY)
    glNormalPointer(GL_FLOAT,3*narr.itemsize,narr)
    glVertexPointer(3, GL_FLOAT, 3*varr.itemsize, varr)
    glDrawElements(GL_TRIANGLES,iarr.size,GL_UNSIGNED_INT,iarr) 

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
    for i in range(-20,20):
        drawLineRow(i)

    glColor3ub(0,0,255)
    glVertex3fv(np.array([0,-20,0]))
    glVertex3fv(np.array([0,20,0]))
    
    glColor3ub(255,255 ,255)
    for i in range(-20,20):
        drawLineCol(i)
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
        global projection,force_shading_mode, frame_mode,animate_mode,storeMesh
        if action==glfw.PRESS:
            if key==glfw.KEY_V:
                projection=(projection+1)%2
            elif key==glfw.KEY_H:#animating mode <-> single mesh mode
                animate_mode=(animate_mode+1)%2
                if animate_mode == 1 :
                    init_arr()
                    if len(storeMesh)==0:
                        animate_store()
                else :
                    init_arr()
                
            elif key==glfw.KEY_Z: #polygone mode 변경
                frame_mode=(frame_mode+1)%2

            elif key==glfw.KEY_S: #shading mode 변경
                force_shading_mode=(force_shading_mode+1)%2

            elif key==glfw.KEY_E: #오브젝트 삭제하기
                storeMesh.pop()
                

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

def drop_callback(window, paths) :
    global file_name
    #print("drop call back start")
    file_name= paths[0].split("\\")
    handle_dropped_file(paths[0])
    #print("handle drop complete")
    print_info(paths[0])

def animate_store():
    handle_dropped_file("./cloud.obj")
    handle_dropped_file("./baby.obj")
    handle_dropped_file("./wing.obj")
    init_arr()

def init_arr():
    global gVertexPosArray,gVertexNorArray,gPosIndexArray,gNorIndexArray,gVertexAvgArray,gFaceIndexArray
    global face_cnt,ver_3_face,ver_4_face ,ver_more_face
    #이전의 값을 모두 초기화
    gVertexPosArray = np.empty((0,3), float) # 해당 평면에서의 vertex 마다의 위치
    gVertexNorArray = np.empty((0,3), float) # 해당 평면에서의 vertex 마다의 normal
    
    gFaceIndexArray = np.empty((0,3), float)
    gVertexAvgArray = []
    #gVertexAvgCntArray = []
    
    gPosIndexArray = [] # vertex 위치 리스트
    gNorIndexArray = [] # normal 벡터 리스트
    
    face_cnt = 0
    ver_3_face = 0
    ver_4_face =0
    ver_more_face =0
    file_name = ""
    #print("handle drop init complete")

def l2norm(v):
    return np.sqrt(np.dot(v, v))

def normalized(v):
    l = l2norm(v)
    return v/l

def handle_dropped_file(obj_path):
    global gVertexPosArray,gVertexNorArray,gPosIndexArray,gNorIndexArray,gVertexAvgArray,gFaceIndexArray
    global storeMesh,face_cnt,ver_3_face,ver_4_face ,ver_more_face, animate_mode
    #print("handle drop start")

    init_arr()
    f = open(obj_path, 'r')

    while True:
        line = f.readline()
        if not line: break
        if len(line)==1 :
            #print("blank line")
            continue
        
        line_split = line.split()

        if line_split[0] == "vn": #normal
            normal_line(line_split)
        elif line_split[0] == 'v': #vertex
            vertex_line(line_split)
        elif line_split[0] == 'f' : #face
            face_cnt+=1 #파이썬은 증감 연산자x
            if len(line_split) == 4 : ver_3_face+=1
            elif len(line_split) == 5 : ver_4_face+=1
            else : ver_more_face+=1
            face_line(line_split)
        
        

    
   

    for i in range(len(gVertexAvgArray)):
        gVertexAvgArray[i]=normalized(gVertexAvgArray[i])
   
    f.close()
    if animate_mode == 1: storeMesh.append([gVertexPosArray,gVertexNorArray,gPosIndexArray,gNorIndexArray,gVertexAvgArray,gFaceIndexArray])
    
    
def normal_line(line_split):
    global gNorIndexArray
    l = [float(line_split[1]),float(line_split[2]),float(line_split[3])]
    tmp_array=[l[0],l[1],l[2]]
    gNorIndexArray.append(tmp_array)
   
 
def vertex_line(line_split):
    global gPosIndexArray,gVertexAvgArray
    l = [float(line_split[1]),float(line_split[2]),float(line_split[3])]
    tmp_array=[l[0],l[1],l[2]]
    gPosIndexArray.append(tmp_array)
    gVertexAvgArray.append([0,0,0])
 

   
        
    

def face_line(line_split):
    global gVertexPosArray,gVertexNorArray,gPosIndexArray,gNorIndexArray,gVertexAvgArray,gFaceIndexArray
    pIndex= gPosIndexArray 
    nIndex = gNorIndexArray
    first_index = line_split[1]
    l = line_split[2:]
    
    #index array에 index추가& face normal array
    for i in range(len(l)-1) :
        t_p = []
        t_n = []

        word_split = first_index.split("/")
        t_p.append(int(word_split[0])-1)
        if len(word_split) == 3 : t_n.append(int(word_split[2])-1)
        else : t_n.append(-1)
    
        word_split = l[i].split("/")
        t_p.append(int(word_split[0])-1)
        if len(word_split) == 3 :t_n.append(int(word_split[2])-1)
        else : t_n.append(-1)
        
        word_split = l[i+1].split("/")
        t_p.append(int(word_split[0])-1)
        if len(word_split) == 3 :t_n.append(int(word_split[2])-1)
        else : t_n.append(-1)

        gFaceIndexArray = np.append(gFaceIndexArray,np.array([t_p],'int32'),axis = 0)
        gFaceIndexArray = gFaceIndexArray.astype(np.int32)
        
        for i in t_p:
            #print([pIndex[i]])
            gVertexPosArray= np.append(gVertexPosArray,np.array([pIndex[i]],'float32'),axis=0)
            gVertexPosArray = gVertexPosArray.astype(np.float32)
            gVertexAvgArray[i][0] += nIndex[int(word_split[2])-1][0]
            gVertexAvgArray[i][1] += nIndex[int(word_split[2])-1][1]
            gVertexAvgArray[i][2] += nIndex[int(word_split[2])-1][2]
            
        #print("-------------------------------------")
        for i in t_n:
            #print([nIndex[i]])
            gVertexNorArray= np.append(gVertexNorArray,np.array([nIndex[i]],'float32'),axis=0)
            gVertexNorArray = gVertexNorArray.astype(np.float32)
        
        
        

    #print("indexarray update complete")
    #print(gVertexPosArray)
    #print(gVertexNorArray)


def print_info(obj):
    global file_name
    #file name
    print("file name: " + str(file_name[-1]))
    #total number of face
    print("total number of face: " +  str(face_cnt))
    #number of face with 3 vertex
    print("number of face with 3 vertex: " +  str(ver_3_face))
    #number of face with 4 vertex
    print("number of face with 4 vertex: " + str(ver_4_face))
    #number of face with more than 4 vertex
    print("number of face with more than 4 vertex: " +  str(ver_more_face))
    

def main():
    
    if not glfw.init(): return
    window=glfw.create_window(800,800,"cg_classAssignment_2",None,None)
    if not window:
        glfw.terminate()
        return

    glfw.set_cursor_pos_callback(window, cursor_pos_callback)
    glfw.set_mouse_button_callback (window, mouse_button_callback)
    glfw.set_key_callback (window, key_callback)
    glfw.set_drop_callback(window,drop_callback)
    glfw.set_scroll_callback (window, scroll_callback)
    
    glfw.make_context_current(window)
    

      
    while not glfw.window_should_close(window):
        glfw.poll_events()
        render()
        glfw.swap_buffers(window)


    glfw.terminate()

if __name__ == "__main__":
    main()
