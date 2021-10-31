import subprocess as sp
import sys
import matplotlib.pyplot as plt
import numpy as np


a = -3
b = 2
m = -0.5
s = 1.5
intervals = 100
samples = [100, 1000, 10000, 100000]

# Uniform distribution
def histo_uniform():
    for i in range(4):
        #hw6.exe(sys.argv[1]) 파일로 인자str(samples[i]), 'uniform' 넣어서 실행
        result = sp.run([sys.argv[1], str(samples[i]), 'uniform'], stdout=sp.PIPE)
        result = result.stdout.decode(encoding='UTF-8')
        x = [float(j) * (b-a) + a for j in result.split()]

        #axis
        plt.subplot(2, len(samples), len(samples)+i+1)
        #title
        plt.title('[samples=' + str(samples[i]) + ']')

        #label
        plt.xlabel('x')
        plt.ylabel('y')

        #histogram set
        plt.hist(x, intervals, density=True, facecolor='blue')

# Gaussian distribution
def histo_gaussian():
    for i in range(4):
        #hw6.exe(sys.argv[1]) 파일로 인자(str(samples[i]), 'gaussian') 넣어서 실행
        result = sp.run([sys.argv[1], str(samples[i]), 'gaussian'], stdout=sp.PIPE)
        result = result.stdout.decode(encoding='UTF-8')
        x = [float(j) * s + m for j in result.split()]

        #axis
        plt.subplot(2, len(samples), len(samples)+i+1)
        #title
        plt.title('[samples=' + str(samples[i]) + ']')

        #label
        plt.xlabel('x')
        plt.ylabel('y')

        #histogram set
        plt.hist(x, intervals, density=True, facecolor='green')


plt.figure(1, figsize=(15, 5))
histo_uniform()
plt.figure(2, figsize=(15, 5))
histo_gaussian()

#show figures 
plt.show()