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

credit_mode = 0
animate_mode = 0
frame_mode = 1

rotate_order = [
                [0,0,0],
                [0,0,0],
                [0,0,0],
                ]
now_motion = 0
motion_change = []
line_arr = []
frame_num = 0
joint_name = []
joint_num = 0
FPS = 0


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
    global camAng_a,camAng_b ,rad,projection, frame_mode, force_shading_modem, credit_mode
    global motion_change, line_arr, joint_num, now_motion, file_name,rotate_order
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

    if credit_mode == 1:
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
        lightColor = (0.8,0.4,0.3,1.)
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
        objectColor = (1.,1,1,1.)
        specularObjectColor = (1.,1.,1.,1.)
        glMaterialfv(GL_FRONT, GL_SHININESS, 15)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, objectColor)
        glMaterialfv(GL_FRONT, GL_SPECULAR, specularObjectColor)


    
    root = 1
    end = 0
    now_line = 0
    motion_index = 0
    obj = 0
    recent_joint = ""
    if file_name != "" and str(file_name[-1]) == "new_file.bvh":
        glScalef(0.01,0.01,0.01)
   
    
    for line in line_arr:
        if line[0] == "OFFSET":
            
            if root != 1 :
                if credit_mode == 0:
                    glTranslatef(float(line[1]),float(line[2]),float(line[3]))
                    glBegin(GL_LINES)
                    glColor3ub(255,255 ,255)
                    glVertex3f(-float(line[1]),-float(line[2]),-float(line[3]))
                    glVertex3f(0,0,0)
                    glEnd()

                else:
                    glTranslatef(float(line[1]),float(line[2]),float(line[3]))
                    glScalef(0.01,0.01,0.01)

                    if obj == 0:
                        
                        #spine
                        glScalef(3,3,3)
                        glTranslatef(-2,-2.8,0)
                        glRotatef(-45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(45,0,0,1)
                        glTranslatef(2,2.8,0)
                        glScalef(1/3,1/3,1/3)

                    elif obj == 1:
                        
                        #neck
                        glScalef(3,7,3)
                        glTranslatef(-2,-3.4,0)
                        glRotatef(-45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(45,0,0,1)
                        glTranslatef(2,3.4,0)
                        glScalef(1/3,1/7,1/3)
                   
                    elif obj == 2:
                        
                        #head
                        glScalef(17,17,17)
                        glRotatef(270,0,1,0)
                        draw_multiple_glDrawElements(0)
                        glRotatef(-270,0,1,0)
                        glScalef(1/15,1/15,1/15)
                        
                          
                    elif obj == 3:
                        
                        #leftarm
                        
                        glRotatef(-45,0,0,1)
                        glScalef(8,3,3)
                        glTranslatef(3,-2,0)
                        glRotatef(45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(-45,0,0,1)
                        glTranslatef(-3,2,0)
                        glScalef(1/8,1/3,1/3)
                        glRotatef(45,0,0,1)
                        
                    elif obj == 4:
                        
                        glScalef(6,3,3)
                        glTranslatef(3,0,0)
                        glRotatef(45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(-45,0,0,1)
                        glTranslatef(-3,0,0)
                        glScalef(1/6,1/3,1/3)
                        
                    elif obj == 5:
                        
                        glScalef(5,3,3)
                        glTranslatef(3,0,0)
                        glRotatef(45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(-45,0,0,1)
                        glTranslatef(-3,0,0)
                        glScalef(1/5,1/3,1/3)
                        
                    elif obj == 6:
                        #leftHand
                        
                        glTranslatef(2,0,0)
                        glRotatef(90,0,1,0)
                        draw_multiple_glDrawElements(2)
                        glRotatef(-90,0,1,0)
                        glTranslatef(-2,0,0)
                        
                    elif obj == 7:
                        
                        #rightArm
                        glRotatef(45,0,0,1)
                        
                        glScalef(8,3,3)
                        glTranslatef(0,-2,0)
                        glRotatef(45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(-45,0,0,1)
                        glTranslatef(0,2,0)
                        glScalef(1/8,1/3,1/3)
                        
                        glRotatef(-45,0,0,1)
                        
                    elif obj == 8:
                        glScalef(6,3,3)
                        glTranslatef(0.5,0,0)
                        glRotatef(45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(-45,0,0,1)
                        glTranslatef(-0.5,0,0)
                        glScalef(1/6,1/3,1/3)
                    elif obj == 9:
                        glScalef(5,3,3)
                        glTranslatef(0.5,0,0)
                        glRotatef(45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(-45,0,0,1)
                        glTranslatef(-0.5,0,0)
                        glScalef(1/5,1/3,1/3)
                    elif obj == 10:
                        #righthand
                        glRotatef(270,0,1,0)
                        draw_multiple_glDrawElements(2)
                        glRotatef(-270,0,1,0)
                    elif obj == 11:
                        #left leg
                        glScalef(3,3,3)
                        glTranslatef(-2.5,2,0)
                        glRotatef(-90,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(90,0,0,1)
                        glTranslatef(2.5,-2,0)
                        glScalef(1/3,1/3,1/3)
                    elif obj == 12:
                        glScalef(3,10,3)
                        glTranslatef(-2.5,0,0)
                        glRotatef(-45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(45,0,0,1)
                        glTranslatef(2.5,0,0)
                        glScalef(1/3,1/10,1/3)
                    elif obj == 13:
                        glScalef(3,7,3)
                        glTranslatef(-2.5,1,0)
                        glRotatef(-45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(45,0,0,1)
                        glTranslatef(2.5,-1,0)
                        glScalef(1/3,1/7,1/3)
                    elif obj == 14:
                        #left foot
                        glScalef(5,5,10)
                        glTranslatef(-0.5,0,0)
                        glRotatef(-90,0,1,0)
                        draw_multiple_glDrawElements(3)
                        glRotatef(90,0,1,0)
                        glTranslatef(0.5,0,0)
                        glScalef(0.2,0.2,0.1)  

                    elif obj == 15:
                        #right leg
                        glScalef(3,3,3)
                        glTranslatef(-1,0,0)
                        draw_multiple_glDrawElements(1)
                        glTranslatef(1,0,0)
                        glScalef(1/3,1/3,1/3)
                    elif obj == 16:
                        glScalef(3,10,3)
                        glTranslatef(-1,0,0)
                        glRotatef(-45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(45,0,0,1)
                        glTranslatef(1,0,0)
                        glScalef(1/3,1/10,1/3)
                    elif obj == 17:
                        glScalef(3,7,3)
                        glTranslatef(-1,1,0)
                        glRotatef(-45,0,0,1)
                        draw_multiple_glDrawElements(1)
                        glRotatef(45,0,0,1)
                        glTranslatef(1,-1,0)
                        glScalef(1/3,1/7,1/3)
                    else :
                        #right foot
                        glScalef(5,5,10)
                        glRotatef(-90,0,1,0)
                        draw_multiple_glDrawElements(3)
                        glRotatef(90,0,1,0)
                        glScalef(0.2,0.2,0.1)  


                    glScalef(100,100,100)  
                    
                    obj+=1
                    


            else :
                 glPushMatrix()
                 if now_motion!=0:
                     glTranslatef(float(motion_change[now_motion-1][motion_index]),float(motion_change[now_motion-1][motion_index+1]),float(motion_change[now_motion-1][motion_index+2]))
                     motion_index += 3

                     
                     glRotatef(float(motion_change[now_motion-1][motion_index]),rotate_order[0][0],rotate_order[0][1],rotate_order[0][2])
                     glRotatef(float(motion_change[now_motion-1][motion_index+1]),rotate_order[1][0],rotate_order[1][1],rotate_order[1][2])
                     glRotatef(float(motion_change[now_motion-1][motion_index+2]),rotate_order[2][0],rotate_order[2][1],rotate_order[2][2])
                     motion_index +=3

         
        elif line[0] == "JOINT":
            end = 0
            root = 0
            recent_joint = line[1]
            glPushMatrix()
            if now_motion!=0:
               
                glRotatef(float(motion_change[now_motion-1][motion_index]),rotate_order[0][0],rotate_order[0][1],rotate_order[0][2])
                glRotatef(float(motion_change[now_motion-1][motion_index+1]),rotate_order[1][0],rotate_order[1][1],rotate_order[1][2])
                glRotatef(float(motion_change[now_motion-1][motion_index+2]),rotate_order[2][0],rotate_order[2][1],rotate_order[2][2])
                motion_index +=3
        elif line[0] == "End":
            end = 1
            recent_joint = "End"
     
        elif end == 1 and line[0] == "}":
            if line_arr[now_line-1][0] != "OFFSET":
                glPopMatrix()


        now_line += 1


    if credit_mode == 1:
        glDisable(GL_LIGHTING)

    

def draw_multiple_glDrawElements(i):
    global storeMesh
    varr = np.array(storeMesh[i][2],'float32') #gPosIndexArray
    narr = np.array(storeMesh[i][4],'float32') #gVertexAvgArray
    iarr = storeMesh[i][5]                     #gFaceIndexArray
    
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
        global projection,force_shading_mode, frame_mode,credit_mode,storeMesh,animate_mode
        if action==glfw.PRESS:
            if key==glfw.KEY_V:
                projection=(projection+1)%2
            elif key==glfw.KEY_C:#credit mode 변경
                credit_mode=(credit_mode+1)%2
                if credit_mode == 1 :
                    handle_dropped_file_obj("./skull.obj")
                    handle_dropped_file_obj("./bone.obj")
                    handle_dropped_file_obj("./hand.obj")
                    handle_dropped_file_obj("./foot.obj")
                    init_arr()

                else :
                    init_arr()
                
            elif key==glfw.KEY_Z: #polygone mode 변경
                frame_mode=(frame_mode+1)%2


            elif key==glfw.KEY_E: #오브젝트 삭제하기
                storeMesh.pop()

            elif key==glfw.KEY_SPACE:
                animate_mode=(animate_mode+1)%2



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
    file_name= paths[0].split("\\")
    if paths[0][-1] == 'j':
        handle_dropped_file_obj(paths[0])
    else :
        handle_dropped_file_bvh(paths[0])
        print_info(paths[0])
    



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

def init_global():
    global frame_num, joint_name, joint_num,FPS,motion_change,line_arr,now_motion,rotete_order
    rotate_order = [
                [0,0,0],
                [0,0,0],
                [0,0,0],
                ]
    now_motion = 0
    motion_change = []
    line_arr = []
    joint_name = []
    joint_num = 0
    frame_num = 0
    FPS = 0


def handle_dropped_file_bvh(bvh_path):
    global frame_num, joint_name, joint_num,FPS,motion_change,line_arr,rotate_order
    init_global()
    f = open(bvh_path,'r')
    motion = 0
    end = 0
    root = 1
    order = 0
    while True:
        line = f.readline()
        if not line: break
        if len(line)==1 :
            #print("blank line")
            continue
        
        line_split = line.split()

        if motion == 1  :
            if line_split[0] == "Frames:":
                frame_num = int(line_split[1])
            elif line_split[0] == "Frame" :
                FPS = 1/float(line_split[2])
            else:
                motion_change.append(line_split)
         

        else :
            if line_split[0] == "ROOT":
                joint_name.append(line_split[1])
            elif line_split[0] == "OFFSET":
                line_arr.append(line_split)
                
                    
            elif line_split[0] == "JOINT" :
                end = 0
                root =0 
                joint_num += 1
                joint_name.append(line_split[1])
                line_arr.append(line_split)

            elif line_split[0] == "MOTION" :
                motion = 1

            elif line_split[0] == "End" :
                end = 1
                line_arr.append(line_split)

            elif end == 1 and line_split[0] == "}":
                line_arr.append(line_split)

            elif line_split[0] == "CHANNELS" and line_split[1] == "3" and order == 0:
                for i in range(2,5):
                    if line_split[i][0] == "X":
                        rotate_order[i-2][0] = 1
                        rotate_order[i-2][1] = 0
                        rotate_order[i-2][2] = 0
                    elif line_split[i][0] == "Y":
                        rotate_order[i-2][0] = 0
                        rotate_order[i-2][1] = 1
                        rotate_order[i-2][2] = 0
                    elif line_split[i][0] == "Z":
                        rotate_order[i-2][0] = 0
                        rotate_order[i-2][1] = 0
                        rotate_order[i-2][2] = 1

                order = 1

                
                
    joint_num += 1
    
    f.close()


    

def handle_dropped_file_obj(obj_path):
    global gVertexPosArray,gVertexNorArray,gPosIndexArray,gNorIndexArray,gVertexAvgArray,gFaceIndexArray
    global storeMesh,face_cnt,ver_3_face,ver_4_face ,ver_more_face, credit_mode
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
    if credit_mode == 1: storeMesh.append([gVertexPosArray,gVertexNorArray,gPosIndexArray,gNorIndexArray,gVertexAvgArray,gFaceIndexArray])
    
    
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
            #print(gVertexAvgArray[i])
            #print(nIndex)
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


def print_info(bvh):
    global file_name,joint_name,joint_num,frame_num
    #file name
    print("file name: " + str(file_name[-1]))
    #total number of frame
    print("number of frame: " +  str(frame_num))
    #FPS
    print("FPS: " + str(FPS))
    #number of joint
    print("number of joint: " + str(joint_num))
    #list of all joint name
    print("list of all joint name: ")
    print(joint_name)
    

def main():
    global now_motion,frame_num,animate_mode

    if not glfw.init(): return
    window=glfw.create_window(800,800,"cg_classAssignment_3",None,None)
    if not window:
        glfw.terminate()
        return

    glfw.set_cursor_pos_callback(window, cursor_pos_callback)
    glfw.set_mouse_button_callback (window, mouse_button_callback)
    glfw.set_key_callback (window, key_callback)
    glfw.set_drop_callback(window,drop_callback)
    glfw.set_scroll_callback (window, scroll_callback)
    
    glfw.make_context_current(window)

    
    glfw.swap_interval(1)
      
    while not glfw.window_should_close(window):
        glfw.poll_events()
        if animate_mode == 1:
            now_motion= (now_motion+1)%(frame_num+1)
        render()
        glfw.swap_buffers(window)


    glfw.terminate()

if __name__ == "__main__":
    main()
