# -*- coding: cp949 -*-
#randint �Լ��� �������� random ����� import���ش�
import random
#while�� ����ؼ� ���ѷ����� ������ش�
while 1 :
    #������ ���� ���� ó���� �ϱ� ���� try/except�� ����Ѵ�
    try:
        #õ��° ����� ���λ������ ���� ����� ����ڿ��� �Է¹޴´� ���� �� ���� �ڿ����� �ƴҰ�� �����޼����� ���� �ٽ� �Է¹޴´�.
        size1=input("input your First Matrix's Rows Size : ")
        while size1<=0 or int(size1)!=size1:
            print("Error!!! please input natural number")
            size1=input("input your First Matrix's Rows Size : ")
            
        size2=input("input your First Matrix's Cols Size : ")
        while size2<=0 or int(size2)!=size2:
            print("Error!!! please input natural number")
            size2=input("input your First Matrix's Cols Size : ")

        #randint�Լ��� ����Ͽ� ������ �޾ƿ� ���μ��� ��� ������� ����� ������ش�
        first=[[0 for i in range(size2)] for j in range(size1) ]
        for i in range(size1):
            for j in range(size2):
                num1=random.randint(0,2)
                first[i][j]=num1

        #ù��° ����� ������ش�
        for i in range(size1):
            for j in range(size2):
                print" %d " %first[i][j],
            print(" ")

        #�ι�° ����� ���λ������ ���� ����� ����ڿ��� �Է¹޴´� ���� �� ���� �ڿ����� �ƴҰ�� �����޼����� ���� �ٽ� �Է¹޴´�.
        size3=input("input your Second Matrix's Rows Size : ")
        while size3<=0 or int(size3)!=size3:
            print("Error!!! please input natural number")
            size3=input("input your Second Matrix's Rows Size : ")
            
        size4=input("input your Second Matrix's Cols Size : ")
        while size4<=0 or int(size4)!=size4:
            print("Error!!! please input natural number")
            size4=input("input your Second Matrix's Cols Size : ")

        #randint�Լ��� ����Ͽ� ������ �޾ƿ� ���μ��� ��� ������� ����� ������ش�
        second=[[0 for i in range(size4)] for j in range(size3) ]
        for i in range(size3):
            for j in range(size4):
                num2=random.randint(0,2)
                second[i][j]=num2

        #�ι�° ����� ������ش�
        for i in range(size3):
            for j in range(size4):
                print" %d " %second[i][j],
            print(" ")
            
            
        #������������ �ٵǴ°��
        if size1==size3 and size2==size4 and size1==size2 :
            #����ڿ��� ���ϴ¿����� �Է¹޴´�
            what1=raw_input("input + or - or * : ")

            #+�� �Է¹������ 
            if what1=="+":
                #for ���� �ι���ø�ؼ� �� ����� ���� �ε����� �κ��� ���ؼ� add ����� ������ش�
                add=[[0 for i in range(size2)] for j in range(size1)]
                for i in range(size1):
                    for j in range(size2):
                        add[i][j]=(first[i][j]+second[i][j])

                #add����� ������ش�        
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %add[i][j],
                    print(" ")

            #-�� �Է¹��� ���            
            elif what1=="-":
                #for ���� �ι���ø�ؼ� �� ����� ���� �ε����� �κ��� ���� sub ����� ������ش�
                sub=[[0 for i in range(size2)] for j in range(size1)]
                for i in range(size1):
                    for j in range(size2):
                        sub[i][j]=(first[i][j]-second[i][j])

                #sub����� ������ش�        
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %sub[i][j],
                    print(" ")
                    
            #*�� �Է¹��� ��� 
            else :
                #for���� 3�� ��ø�ؼ� ��İ��� ������ �� mul����� ���� ���� �־��ش�.   
                mul=[]
                for i in range(size1):
                    mul.append([])
                    for j in range(size4):
                        add=0
                        for k in range(size2):
                            add=add+(int(first[i][k])*int(second[k][j]))
                        mul[i].append(add)
                        
                #mul����� ������ش�
                for i in range(size1):
                    for j in range(size4):
                        print" %d " %mul[i][j],
                    print(" ")

        #������ �Ǵ°��
        elif size2==size3 and size1!=size3 :
            #����ڿ��� ���ϴ¿����� �Է¹޴´�
            what2=raw_input("input * : ")

            #for���� 3�� ��ø�ؼ� ��İ��� ������ �� mul����� ���� ���� �־��ش�.
            mul=[]
            for i in range(size1):
                mul.append([])
                for j in range(size4):
                    add=0
                    for k in range(size2):
                        add=add+(int(first[i][k])*int(second[k][j]))
                    mul[i].append(add)

            #mul����� ������ش�        
            for i in range(size1):
                for j in range(size4):
                    print" %d " %mul[i][j],
                print(" ")

        #�ƹ��͵� �ȵǴ� ��� �����޼����� ���� �ٽ� ���ѷ����� ���� while���� ó������ ���ư���
        elif size2!=size3 and size1!=size3 :
            print("Error---you can't do anything")
            continue


        #���������� �Ǵ°��
        elif size1==size3 and size2==size4 and size2!=size3 :
            #����ڿ��� ���ϴ¿����� �Է¹޴´�
            what3=raw_input("input + or - : ")

            #+�� �Է¹������ 
            if what3=="+":
                #for ���� �ι���ø�ؼ� �� ����� ���� �ε����� �κ��� ���ؼ� add ����� ������ش�
                add=[[0 for i in range(size2)] for j in range(size1)]
                for i in range(size1):
                    for j in range(size2):
                        add[i][j]=(first[i][j]+second[i][j])

                #add����� ������ش�        
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %add[i][j],
                    print(" ")

            #-�� �Է¹������
            elif what3=="-":
                #for ���� �ι���ø�ؼ� �� ����� ���� �ε����� �κ��� ���� sub ����� ������ش�
                sub=[[0 for i in range(size2)] for j in range(size1)]
                for i in range(size1):
                    for j in range(size2):
                        sub[i][j]=(first[i][j]-second[i][j])

                #sub����� ������ش�
                for i in range(size3):
                    for j in range(size4):
                        print" %d " %sub[i][j],
                    print(" ")
                    

        #������ ������� ����
        q=raw_input("Continue? (o or x) : ")
        if q=="o:":
            pass
        elif q=="x":
            break
            
    #except�� ���� ó���� ���ش�.
    except NameError:
        print("<ERROR> Name error")



