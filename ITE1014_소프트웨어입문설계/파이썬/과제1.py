#-*- coding: cp949 -*- 사용자가 어떤 연산을 하고싶은지 입력을 받는다.
a=raw_input('what to do? ')
#첫번째 피연산자 값을 입력받는다.
b=input('first operand ')
#두뻔째 피연산자 값을 입력받는다.
c=input('second operand ')
#사용자가 +나 add 를 입력했을 경우 더하기 연산을 하고 소수점 두자리 실수로 나타낸다.
if a=='+'or a=='add':
    print('result is %0.2f') %(b+c)
#사용자가 -나 sub 를 입력했을 경우 b-c로 계산할지 c-b로 계산할지 질문하고 그 응답에 따라 빼기 연산을 하고 소수점 두자리 실수로 나타낸다.
elif a=='-'or a=='sub':
    e=raw_input('first-second or second-first? (f-s or s-f) ')
    if e=='s-f':
        print('result is %0.2f') %(c-b)
    elif e=='f-s':
        print('result is %0.2f') %(b-c)
#사용자가 *나 mul 를 입력했을 경우 곱하기 연산을 하고 소수점 두자리 실수로 나타낸다.
elif a=='*'or a=='mul':
    print('result is %0.2f') %(b*c)
#사용자가 /나 div 를 입력했을 경우 b/c로 계산할지 c/b로 계산할지 질문하고 그 응답에 따라 나누기 연산을 하고 소수점 두자리 실수로 나타낸다. 만약 나누는 수가 0일경우 오류메세지를 띄운다.
elif a=='/'or a=='div':
    d=raw_input('first/second or second/first? (f/s or s/f) ')
    if d=='s/f':
        if b!=0:
            print('result if %0.2f') %(c/b)
        else :
            print("can't divide by zero")
    elif d=='f/s':
        if c!=0:
            print('result is %0.2f') %(b/c)
        else :
            print("can't divide by zero")
    
