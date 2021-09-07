def average(b):
        sum = 0
        for i in b:
            sum = sum + float(i)
        return sum/len(b)   
b=[]
print('input numbers(input result to stop)')
while 1:
    c=raw_input()
    if c!= 'result':
        float(c)
        b.append(c)
        continue
    else :
        d=average(b)
        print('%0.4f'%d)
        break
        
        



    
    
