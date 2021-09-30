# -*- coding: cp949 -*-
#randint 함수를 쓰기위해 random 모듈을 import해준다
import random
#while을 사용해서 무한루프를 만들어준다
while 1 :
    #에러가 날때 예외 처리를 하기 위해 try/except를 사용한다
    try:
        #천번째 행령을 가로사이즈와 세로 사이즈를 사용자에게 입력받는다 만약 그 값이 자연수가 아닐경우 오류메세지를 띄우고 다시 입력받는다.
        size1=input("input your First Matrix's Rows Size : ")
        while size1<=0 or int(size1)!=size1:
            print("Error!!! please input natural number")
            size1=input("input your First Matrix's Rows Size : ")
            
        size2=input("input your First Matrix's Cols Size : ")
        while size2<=0 or int(size2)!=size2:
            print("Error!!! please input natural number")
            size2=input("input your First Matrix's Cols Size : ")

        #randint함수를 사용하여 위에서 받아온 가로세로 행렬 사이즈로 행렬을 만들어준다
        first=[[0 for i in range(size2)] for j in range(size1) ]
        for i in range(size1):
            for j in range(size2):
                num1=random.randint(0,2)
                first[i][j]=num1

        #첫번째 행렬을 출력해준다
        for i in range(size1):
            for j in range(size2):
                print" %d " %first[i][j],
            print(" ")

        #두번째 행령을 가로사이즈와 세로 사이즈를 사용자에게 입력받는다 만약 그 값이 자연수가 아닐경우 오류메세지를 띄우고 다시 입력받는다.
        size3=input("input your Second Matrix's Rows Size : ")
        while size3<=0 or int(size3)!=size3:
            print("Error!!! please input natural number")
            size3=input("input your Second Matrix's Rows Size : ")
            
        size4=input("input your Second Matrix's Cols Size : ")
        while size4<=0 or int(size4)!=size4:
            print("Error!!! please input natural number")
            size4=input("input your Second Matrix's Cols Size : ")

        #randint함수를 사용하여 위에서 받아온 가로세로 행렬 사이즈로 행렬을 만들어준다
        second=[[0 for i in range(size4)] for j in range(size3) ]
        for i in range(size3):
            for j in range(size4):
                num2=random.randint(0,2)
                second[i][j]=num2

        #두번째 행렬을 출력해준다
        for i in range(size3):
            for j in range(size4):
                print" %d " %second[i][j],
            print(" ")
            
            
        #덧셈뺄셈곱셈 다되는경우
        if size1==size3 and size2==size4 and size1==size2 :
            #사용자에게 원하는연산을 입력받는다
            what1=raw_input("input + or - or * : ")

            #+를 입력받은경우 
            if what1=="+":
                #for 문을 두번중첩해서 두 행렬의 동일 인덱스인 부분을 더해서 add 행렬을 만들어준다
                add=[[0 for i in range(size2)] for j in range(size1)]
                for i in range(size1):
                    for j in range(size2):
                        add[i][j]=(first[i][j]+second[i][j])

                #add행렬을 출력해준다        
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %add[i][j],
                    print(" ")

            #-를 입력받은 경우            
            elif what1=="-":
                #for 문을 두번중첩해서 두 행렬의 동일 인덱스인 부분을 빼서 sub 행렬을 만들어준다
                sub=[[0 for i in range(size2)] for j in range(size1)]
                for i in range(size1):
                    for j in range(size2):
                        sub[i][j]=(first[i][j]-second[i][j])

                #sub행렬을 출력해준다        
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %sub[i][j],
                    print(" ")
                    
            #*를 입력받은 경우 
            else :
                #for문을 3번 중첩해서 행렬곱을 실행한 뒤 mul행렬을 만들어서 값을 넣어준다.   
                mul=[]
                for i in range(size1):
                    mul.append([])
                    for j in range(size4):
                        add=0
                        for k in range(size2):
                            add=add+(int(first[i][k])*int(second[k][j]))
                        mul[i].append(add)
                        
                #mul행렬을 출력해준다
                for i in range(size1):
                    for j in range(size4):
                        print" %d " %mul[i][j],
                    print(" ")

        #곱셈만 되는경우
        elif size2==size3 and size1!=size3 :
            #사용자에게 원하는연산을 입력받는다
            what2=raw_input("input * : ")

            #for문을 3번 중첩해서 행렬곱을 실행한 뒤 mul행렬을 만들어서 값을 넣어준다.
            mul=[]
            for i in range(size1):
                mul.append([])
                for j in range(size4):
                    add=0
                    for k in range(size2):
                        add=add+(int(first[i][k])*int(second[k][j]))
                    mul[i].append(add)

            #mul행렬을 출력해준다        
            for i in range(size1):
                for j in range(size4):
                    print" %d " %mul[i][j],
                print(" ")

        #아무것도 안되는 경우 에러메세지를 띄우고 다시 무한루프를 통해 while문의 처음으로 돌아간다
        elif size2!=size3 and size1!=size3 :
            print("Error---you can't do anything")
            continue


        #덧셈뺄샘만 되는경우
        elif size1==size3 and size2==size4 and size2!=size3 :
            #사용자에게 원하는연산을 입력받는다
            what3=raw_input("input + or - : ")

            #+를 입력받은경우 
            if what3=="+":
                #for 문을 두번중첩해서 두 행렬의 동일 인덱스인 부분을 더해서 add 행렬을 만들어준다
                add=[[0 for i in range(size2)] for j in range(size1)]
                for i in range(size1):
                    for j in range(size2):
                        add[i][j]=(first[i][j]+second[i][j])

                #add행렬을 출력해준다        
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %add[i][j],
                    print(" ")

            #-를 입력받은경우
            elif what3=="-":
                #for 문을 두번중첩해서 두 행렬의 동일 인덱스인 부분을 빼서 sub 행렬을 만들어준다
                sub=[[0 for i in range(size2)] for j in range(size1)]
                for i in range(size1):
                    for j in range(size2):
                        sub[i][j]=(first[i][j]-second[i][j])

                #sub행렬을 출력해준다
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %sub[i][j],
                    print(" ")
                    

        #끝낼지 계속할지 질문
        q=raw_input("Continue? (o or x) : ")
        if q=="o:":
            pass
        elif q=="x":
            break
            
    #except로 예외 처리를 해준다.
    except NameError:
        print("<ERROR> Name error")



