
while 1:
    try:
        print("     Calculator     ")
        fir=input("First operand : ")
        sec=input("Second operand : ")
        fir=float(fir)
    
        print("%0.2f + %0.2f = %0.2f" %(fir,sec,(fir+sec)))
        print("%0.2f - %0.2f = %0.2f" %(fir,sec,(fir-sec)))
        print("%0.2f * %0.2f = %0.2f" %(fir,sec,(fir*sec)))
        print("%0.2f / %0.2f = %0.2f" %(fir,sec,(fir/sec)))
    except NameError:
        print("<ERROR> Name error")
    except ZeroDivisionError :
        print("<ERROR> Zero division error")
    except IndexError:
        print("<ERROR> Index error")
    except:
        print("<ERROR> 'Some' error happened, but don't know what it is")

    
