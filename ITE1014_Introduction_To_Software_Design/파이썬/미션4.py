while 1:
    
    a=input('Menu\n1.99dan 2.exit\n')
    if a==1:
        b=input('input dan : ')
        num=0
        while num<=9:
            num=num+1
            print('%d * %d = %d' %(b,num,b*num))
    else:
        break
            

