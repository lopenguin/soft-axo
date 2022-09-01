# Neil Janwani 8/19/22

import matplotlib.pyplot as plt
import numpy as np
import csv

START = 40000
END = 40500
N = END - START
FILENAME = r'q.csv'
# PROFILE = [np.cos(2 * np.pi * x) + np.cos(9 * 2 * np.pi * x) \
#            for x in np.linspace(0, 1, N)]
# TIME = [x for x in range(len(PROFILE))]

PROFILE = []
TIME = []
with open(FILENAME) as file:
    reader = csv.reader(file)
    for i, row in enumerate(reader):
        if i > START:
            PROFILE.append(float(row[1]))
            TIME.append(float(row[0]))
        if i > END:
            print(row)
            break


def DFT(x):
    """
    Function to calculate the 
    discrete Fourier Transform 
    of a 1D real-valued signal x
    """

    _N = len(x)
    n = np.linspace(0, _N, _N)
    k = n.reshape((_N,1))
    e = np.exp(-2j * np.pi * k * n / _N)
    
    X = np.dot(e, x)
    
    return X

dft = DFT(PROFILE)[:50]
bar = np.array([np.linalg.norm(x) for x in dft]) * 2 / len(PROFILE)
angles = np.array([np.arctan2(np.imag(x), np.real(x)) for x in dft])
# print(dft)
plt.subplot(3,1,1)
plt.bar([x for x in range(len(bar))], bar)
plt.subplot(3,1,2)
plt.plot(TIME, PROFILE)
plt.subplot(3,1,3)
y = []
x = []
for j, t in enumerate(np.linspace(0,1,N)):
    y.append(0)
    x.append(t)
    for i, elem in enumerate(bar):
        y[j] += elem * np.cos(2 * np.pi * i * t + angles[i])

plt.scatter(x, y, s = 1)
plt.show()