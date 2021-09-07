import random
size1=input("input your First Matrix's Rows Size : ")
size2=input("input your First Matrix's Cols Size : ")
first=[[0 for i in range(size2)] for j in range(size1)]
for i in range(size1):
    for j in range(size2):
        num1=random.randint(0,2)
        first[i][j]=num1
for i in range(size1):
    for j in range(size2):
        print" %d " %first[i][j],
    print(" ")

size3=input("input your Second Matrix's Rows Size : ")
size4=input("input your Second Matrix's Cols Size : ")
second=[[0 for i in range(size4)] for j in range(size3) ]
for i in range(size3):
    for j in range(size4):
        num2=random.randint(0,2)
        second[i][j]=num2
for i in range(size3):
    for j in range(size4):
        print" %d " %second[i][j],
    print(" ")

if (size1==size3) and (size2==size4):
    what1=raw_input("input + or - or * : ")
    if what1=="+":
                add=[[0 for i in range(size2)] for j in range(size1) ]
                for i in range(size1):
                    for j in range(size2):
                        add[i][j]=(first[i][j]+second[i][j])
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %add[i][j],
                    print(" ")
                
                        
    elif what1=="-":
                sub=[[0 for i in range(size2)] for j in range(size1) ]
                for i in range(size1):
                    for j in range(size2):
                        sub[i][j]=(first[i][j]-second[i][j])
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %sub[i][j],
                    print(" ")
    else :
        mul=[]
        for i in range(size1):
            mul.append([])
            for j in range(size4):
                add=0
                for k in range(size2):
                    add=add+(int(first[i][k])*int(second[k][j]))
                mul[i].append(add)

        for i in range(size1):
                for j in range(size4):
                    print" %d " %mul[i][j],
                print(" ")
            

                
