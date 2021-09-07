import numpy as np

a=np.arange(2,27)
print(a)
print()
a=a.reshape(5,5)
print(a)
print()

a[1:4,1:4]=0
print(a)
print()

a=a[:,:]@a[:,:]
print(a)
print()

b=a[0,:]*a[0,:]
print(np.sqrt(sum(b)))
print()

