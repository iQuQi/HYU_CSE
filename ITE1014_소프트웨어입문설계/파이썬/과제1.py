#-*- coding: cp949 -*- ����ڰ� � ������ �ϰ������ �Է��� �޴´�.
a=raw_input('what to do? ')
#ù��° �ǿ����� ���� �Է¹޴´�.
b=input('first operand ')
#�λ�° �ǿ����� ���� �Է¹޴´�.
c=input('second operand ')
#����ڰ� +�� add �� �Է����� ��� ���ϱ� ������ �ϰ� �Ҽ��� ���ڸ� �Ǽ��� ��Ÿ����.
if a=='+'or a=='add':
    print('result is %0.2f') %(b+c)
#����ڰ� -�� sub �� �Է����� ��� b-c�� ������� c-b�� ������� �����ϰ� �� ���信 ���� ���� ������ �ϰ� �Ҽ��� ���ڸ� �Ǽ��� ��Ÿ����.
elif a=='-'or a=='sub':
    e=raw_input('first-second or second-first? (f-s or s-f) ')
    if e=='s-f':
        print('result is %0.2f') %(c-b)
    elif e=='f-s':
        print('result is %0.2f') %(b-c)
#����ڰ� *�� mul �� �Է����� ��� ���ϱ� ������ �ϰ� �Ҽ��� ���ڸ� �Ǽ��� ��Ÿ����.
elif a=='*'or a=='mul':
    print('result is %0.2f') %(b*c)
#����ڰ� /�� div �� �Է����� ��� b/c�� ������� c/b�� ������� �����ϰ� �� ���信 ���� ������ ������ �ϰ� �Ҽ��� ���ڸ� �Ǽ��� ��Ÿ����. ���� ������ ���� 0�ϰ�� �����޼����� ����.
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
    
