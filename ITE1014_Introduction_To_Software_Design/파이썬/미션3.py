a=input('input number\n')
if type(a)is float:
    print('this number is float')
elif type(a)is int:
    if a < 0 :
        print('this number is negative')
    elif a == 0 :
        print('this number is 0')
    else :
        if a%2!=0:
            print('this is odd')
        else :
            print('this is even')
            
