import random
import operator

max_x = 5
max_y = 5
start = -1
end = -1

q_table = []
play_map = list()

gamma = 0.9
bomb = -100
goal = 100
bonus = 1

visited = []
    
def init_table():
    global max_x,max_y
    dx = [0,1,0,-1]
    dy = [1,0,-1,0] 
    table = list()
    for i in range(max_x):
        for j in range(max_y):
            tmp_dict = dict()
            #down, right, up ,left - Neighbors
            for k in range(4):
                if (i+dx[k]<0 or i+dx[k]>=max_x or j+dy[k]<0 or j+dy[k]>=max_y):
                    continue
                neighbor = (i+dx[k])*5+(j+dy[k]) 
                tmp_dict[neighbor] = 0
            table.append(tmp_dict)

    return table

def read_file(file_name):
    global play_map, start,max_y,max_x

    #Open the file for reading and get input.
    with open(file_name,'r') as f:
        while True:
            line = f.readline().split()
            if not line:
                break
            play_map.append(line[0])

    #Finding start point.       
    for i in range(max_x):
        for j in range(max_y):
            if play_map[i][j] == 'S':
                start = 5*i+j
       
    return



def start_game():
    global max_q,end,q_table,play_map,gamma,bomb,bonus,goal,start
    #Start Point
    now_point = start
    while True:

        #Select next point randomly
    
        next_point_list=list(q_table[now_point].keys())
        random_num=random.randrange(0,len(next_point_list))
        next_point = next_point_list[random_num]


        #Init value
        done = False
        new_q = 0
        delayed_q = gamma * max(list(q_table[next_point].values()))
        #If Goal Point
        if play_map[next_point//5][next_point%5] == 'G':
            new_q = goal + delayed_q
            done = True
            end = next_point

        #If Bomb -> Game Over
        elif play_map[next_point//5][next_point%5] == 'B':
            done = True
            new_q = bomb + delayed_q

        #If Bonus Point
        elif play_map[next_point//5][next_point%5] == 'T':    
            new_q = bonus + delayed_q

        #If Normal Point
        elif play_map[next_point//5][next_point%5] == 'P':
            new_q = delayed_q

        #Update Q value
        q_table[now_point][next_point] = new_q

        #If Game is over
        if done:
            return
        
        #Move to next point
        now_point = next_point
        
def write_file(file_name):
    global end,start,q_table
    '''
    cnt=-1
    for i in q_table:
        cnt+=1
        print(str(cnt)+":" +str(i))
    '''
    max_q = sorted(q_table[start].items(),reverse=True, key=operator.itemgetter(1))[0][1]
    
    with open(file_name,'w') as fp:
        now_point = start
        while True:
            fp.write(str(now_point) + " ")
            if now_point == end : 
                break

            now_point=sorted(q_table[now_point].items(),reverse=True, key=operator.itemgetter(1))[0][0]
           
              
        fp.write("\n"+str(max_q))
    
    print("finish")
    return



def assignment_2():
    global q_table

    #init Q table
    q_table = init_table()

    #read file
    read_file('input.txt')
    #iteration 
    for i in range(10000):
        start_game()

    #write file
    write_file('output.txt')

if __name__ == '__main__':
    assignment_2()