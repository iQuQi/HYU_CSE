# -*- coding: cp949 -*-
def printsum(list1):#���ϱ� ��Ģ���꿡 ����� �Լ��� �������ش�
    sum=0#sum ������ 0�� �־�����
    for i in list1:#for ���� ����Ͽ� list1���� �ϳ��� ���� ��  float���� type�� �ٲٰ� �����ش�
        sum = sum + float(i)
    return sum#list1���� ���� �ٴ��� ���� �����Ѵ�

def printsub(list2):#���� ��Ģ���꿡 ����� �Լ��� �������ش�
    float(list2[0])#list�ȿ� ������ ����ȭ �Ǳ� ������ �ٽ� float���� type�� �ٲ��ش�
    float(list2[1])
    sub=list2[0]-list2[1]#list2���� ���� ù��° ������ �ι�° ���� ���ش�
    return sub#��� ���� �����Ѵ�
    
def printmul(list3):#���ϱ� ��Ģ���꿡 ����� �Լ��� �������ش�
    mul=1#mul������ 1�� �־�����
    for k in list3:#for���� ����Ͽ� list3���� �ϳ��� ���� �� float���� �� ���� type�� �ٲ��� ���ʷ� �����ش�
        mul=mul*float(k)
    return mul#��� ���� �����Ѵ�

def printdiv(l,m):#������ ��Ģ���꿡 ����� �Լ��� �������ش�
    div=l/m
    return div#��� ���� �����Ѵ�

#����Ʈ�� ������ش�
list1=[]
list2=[]
list3=[]



while 1 :#while�� ����� ���� ������ ������ش�
    #�޴��� ���� ����ڿ��� ������  ��Ģ������ �Է¹޴´�
    a=input('Calculator\nwhat to do?\n1.add 2.sub 3.mul 4.div 0.quit\n')
    if a==1:#if�� ����ؼ� a=1�϶� ���ϱ� ��Ģ������ �����Ѵ�
        print('input as many as you want(input result to quit)')#���ϴ¸�ŭ ���ڸ� �Է��ϰ� ������� �ް������ result�� �Է��϶�� ����ڿ��� �����ش�
        while 1 :#while�� �׻� ���� ������ ����Ͽ� ���ѷ����� ������ش�
            b=raw_input()#����ڿ��� �Է��� �޴´�
            if b!= 'result' :#�Է¹��� ���� result�� �ƴҰ�� �� ������ ���ʷ� list1�� �־��ش�
                float(b)
                list1.append(b)
                continue#����ڰ� ���ϴ� ��ŭ�Է��� �� �ֵ��� continue�� �����
            else : #else�� ����ؼ� �Է¹��� ����result�� �ƴҶ� ���ϱ� �Լ��� �����ϰ� �� ���� �Ҽ�����°�ڸ����� ����� �� break�� ������
                c=printsum(list1)
                print('%0.2f' %c)
                break     

    elif a==2:#a=2�϶� ���� ��Ģ������ �����Ѵ�
        #����ڿ��� ���⿡ ����� �� ���� �Է¹޴´�
        d=input('first operand: ')
        e=input('second operand: ')
        #�Է¹��� �� ���� ���ʷ� list2�� �־��ش�
        list2.append(d)
        list2.append(e)
        #�տ� �������� ���� �Լ��� �ҷ����� �� ���� �Ҽ��� ���ڸ� �Ǽ��� ����Ѵ�
        f=printsub(list2)
        print('%0.2f'%f)

    elif a==3:#a=3�϶� ���ϱ� ��Ģ������ �����Ѵ�
        print('input  as many as you want(input result to quit)')#���ϴ� ��ŭ ���ڸ� �Է��ϰ� ������� �ް� ������ result�� �Է��� ���� ����ڿ��� �˷��ش�
        while 1:#while �� �׻� ���� ������ ����Ͽ� ���� ������ ������ش�
            g=raw_input()#����ڿ��� �Է��� �޴´�
            if g!= 'result' :#�Է¹������� result�� �ƴҰ�� �� ������ ���ʷ� list3�� �־��ش�
                float(g)
                list3.append(g)
                continue#����ڰ� ���ϴ� ��ŭ �Է��� �� �ֵ��� continue�� ����Ѵ�
            else:#�Է¹��� ���� result�� �ƴҰ�� ���ϱ� �Լ��� �����ϰ� �� ���� �Ҽ��� ���ڸ� �Ǽ��� ��Ÿ������ �� �� break�� �����ش�
                h=printmul(list3)
                print('%0.2f'%h)
                break

    elif a==4:#a=4�϶� ������ ��Ģ������ �����Ѵ�
        #����ڿ��� �����⿡ ����� �� ���� �Է¹޴´�
        l=input('first operand: ')
        m=input('second operand: ')
        if m!=0:#m�� 0�� �ƴҰ�� ������ �Լ��� �ҷ����� �� ���� �Ҽ��� ���ڸ� �Ǽ��� ������ش�
            n=printdiv(l,m)
            print('%0.2f'%n)
        elif m==0:#������ ���� 0�ϰ�� ������ ���ٴ� �޼����� ����ش�
            print("can't divide by zero!!!")
        
    elif a==0:#a=5�ϰ�� break�� �����ش�
        break
    else: #a�� ���� 1,2,3,4,5 �� �ϳ��� �ƴ� �ٸ� ���� ��� �߸��� ����̶�� �޼����� ����ش�.
        print('Wrong command!!')
    
