# -*- coding: cp949 -*-
def printsum(list1):#더하기 사칙연산에 사용할 함수를 선언해준다
    sum=0#sum 변수에 0을 넣어준후
    for i in list1:#for 문을 사용하여 list1에서 하나씩 꺼낸 후  float으로 type을 바꾸고 더해준다
        sum = sum + float(i)
    return sum#list1에서 꺼내 다더한 값을 리턴한다

def printsub(list2):#빼기 사칙연산에 사용할 함수를 선언해준다
    float(list2[0])#list안에 넣으면 문자화 되기 때문에 다시 float으로 type을 바꿔준다
    float(list2[1])
    sub=list2[0]-list2[1]#list2에서 꺼낸 첫번째 값에서 두번째 값을 빼준다
    return sub#결과 값을 리턴한다
    
def printmul(list3):#곱하기 사칙연산에 사용할 함수를 선언해준다
    mul=1#mul변수에 1을 넣어준후
    for k in list3:#for문을 사용하여 list3에서 하나씩 꺼낸 후 float으로 그 값의 type을 바꾼후 차례로 곱해준다
        mul=mul*float(k)
    return mul#결과 값을 리턴한다

def printdiv(l,m):#나누기 사칙연산에 사용할 함수를 선언해준다
    div=l/m
    return div#결과 값을 리턴한다

#리스트를 만들어준다
list1=[]
list2=[]
list3=[]



while 1 :#while을 사용해 무한 루프를 만들어준다
    #메뉴를 띄우고 사용자에게 실행할  사칙연산을 입력받는다
    a=input('Calculator\nwhat to do?\n1.add 2.sub 3.mul 4.div 0.quit\n')
    if a==1:#if을 사용해서 a=1일때 더하기 사칙연산을 실행한다
        print('input as many as you want(input result to quit)')#원하는만큼 숫자를 입력하고 결과값을 받고싶을때 result를 입력하라고 사용자에게 전해준다
        while 1 :#while과 항상 참인 문장을 사용하여 무한루프를 만들어준다
            b=raw_input()#사용자에게 입력을 받는다
            if b!= 'result' :#입력받은 값이 result가 아닐경우 그 값들을 차례로 list1에 넣어준다
                float(b)
                list1.append(b)
                continue#사용자가 원하는 만큼입력할 수 있도록 continue를 사용한
            else : #else를 사용해서 입력받은 값이result가 아닐때 더하기 함수를 실행하고 그 값을 소수점둘째자리까지 출력한 후 break로 나간다
                c=printsum(list1)
                print('%0.2f' %c)
                break     

    elif a==2:#a=2일때 빼기 사칙연산을 실행한다
        #사용자에게 빼기에 사용할 두 수를 입력받는다
        d=input('first operand: ')
        e=input('second operand: ')
        #입력받은 두 수를 차례로 list2에 넣어준다
        list2.append(d)
        list2.append(e)
        #앞에 선언해준 빼기 함수를 불러온후 그 값을 소수점 두자리 실수로 출력한다
        f=printsub(list2)
        print('%0.2f'%f)

    elif a==3:#a=3일때 곱하기 사칙연산을 실행한다
        print('input  as many as you want(input result to quit)')#원하는 만큼 숫자를 입력하고 결과값을 받고 싶을때 result를 입력할 것을 사용자에게 알려준다
        while 1:#while 과 항상 참인 문장을 사용하여 무한 루프를 만들어준다
            g=raw_input()#사용자에게 입력을 받는다
            if g!= 'result' :#입력받은값이 result가 아닐경우 그 값들을 차례로 list3에 넣어준다
                float(g)
                list3.append(g)
                continue#사용자가 원하는 만큼 입력할 수 있도록 continue를 사용한다
            else:#입력받은 값이 result가 아닐경우 곱하기 함수를 실행하고 그 값을 소수점 두자리 실수로 나타내도록 한 후 break로 나가준다
                h=printmul(list3)
                print('%0.2f'%h)
                break

    elif a==4:#a=4일때 나누기 사칙연산을 실행한다
        #사용자에게 나누기에 사용할 두 수를 입력받는다
        l=input('first operand: ')
        m=input('second operand: ')
        if m!=0:#m이 0이 아닐경우 나누기 함수를 불러온후 그 값을 소수점 두자리 실수로 출력해준다
            n=printdiv(l,m)
            print('%0.2f'%n)
        elif m==0:#나누는 수가 0일경우 나눌수 없다는 메세지를 띄워준다
            print("can't divide by zero!!!")
        
    elif a==0:#a=5일경우 break로 끝내준다
        break
    else: #a의 값이 1,2,3,4,5 중 하나가 아닌 다른 값일 경우 잘못된 명령이라는 메세지를 띄워준다.
        print('Wrong command!!')
    
