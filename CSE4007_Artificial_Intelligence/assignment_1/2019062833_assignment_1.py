import random
input = []
solution = ['no solution']


def csp(N,depth,node):
    global solution

    #If N is 2 or 3, there is no answer.
    if(N==2 or N==3):
        return

    #If solution is already found, then return
    if solution[0] != 'no solution':
        return 

    if depth == N:
        #Find solution
        solution = node
        return

    #Validate and Expand node
    for i in range(1,N+1):
        #Pruning
        if attack_pair(node+[i],N)==0:
            csp(N,depth+1,node+[i])


def bfs(N):
    global solution
    #If N is 2 or 3, there is no answer.
    if(N==2 or N==3):
        return

    #Create queue and append empty node
    queue = []
    queue.append([])

    #Run bfs until queue is empty
    while queue:
        #Pop node
        node = queue[0]
        del queue[0]
        #If length of Node is N, all the queen's positions are decided.
        if len(node) == N :
            #Check if all the queens don't attack each other.
            if attack_pair(node,N)==0 :
                solution = node
                return
            else :
                continue

        #If not, expand node 
        else : 
            expand = []
            for i in range(1,N+1):
                expand.append(node+[i])
            queue = queue + expand


def hc(N):
    global solution

    #If N is 2 or 3, there is no answer.
    if(N==2 or N==3):
        return

    while True:
        node = []
        #Arrange the queens randomly.(Random restart)
        for i in range(N):
            num = random.randint(1,N+1)
            node.append(num)
        
        #Find the best movement in the current state.
        while True:
            min_pair = attack_pair(node,N)
            next_node = node
            for i in range(N):
                for j in range(1,N+1):
                    #Find the number of attack pairs for each movement.
                    test = node[0:i]+[j]+node[i+1:N]
                    pair = attack_pair(test,N)

                    #Find Solution
                    if pair == 0 :
                        solution = test
                        return

                    elif min_pair > pair:
                        min_pair = pair
                        next_node = test
            
            #If it's stuck, stop searching.
            if(next_node == node):
                break
            #Else Continue
            else:
                node = next_node
   

def attack_pair(node,N):
    attack = 0
    queens= []
    count = 1
    valid = list(range(1,N+1))

    #Check if it's in the same row.
    for i in node:
        if i in valid:
            valid.remove(i)
        else :
            attack = attack + 1 

        #The coordinates of the queens. (x,y)
        queens.append([count,i])
        count = count + 1

    #Check if the queens are on the same diagonal line.
    while queens:
        x, y = queens[0]
        del queens[0]
        for x_, y_ in queens:
            #Attack each other
            if abs(x-x_) == abs(y-y_):
                attack = attack + 1 

    return attack


def write_file(n,method):
    global solution
    with open('%s_%s_output.txt'%(str(n),method),'w') as fp:
        for i in solution:
            fp.write(str(i)+" ")
    return


def read_file():    
    global input

    with open("input.txt",'r') as fp:
        while True:
            line = fp.readline().split()
            if not line:
                break
            input.append(line)


def main():
    global solution,input
    read_file()
    for n,method in input:
        n = int(n)
        if method =='bfs':
            bfs(n)
        elif method =='hc':
            hc(n)
        elif method =='csp':
            csp(n,0,[])
        else :
            continue
        write_file(n,method)
        solution = ['no solution']
        



if __name__ == '__main__':
    main()