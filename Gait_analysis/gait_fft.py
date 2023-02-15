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
            q0 = float(row[1])
            q1 = float(row[2])
            q2 = float(row[3])
            q3 = float(row[4])
            # PROFILE.append(float(row[1]))
            # PROFILE.append(np.arctan2(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2)))
            # PROFILE.append(np.arctan2(2*(q0*q3+q1*q2), 1 - 2*(q2*q2 + q3*q3)))
            PROFILE.append(np.arcsin(2*(float(row[1])*float(row[3]) - float(row[2])*float(row[4])))) # RIGHT ONE!
            TIME.append(float(row[0]))
        if i > END:
            print(row)
            break

# differentiate
# DERIVATIVE = []
# for i in range(len(PROFILE)-1):
#     DERIVATIVE.append((PROFILE[i+1] - PROFILE[i]) / (TIME[i+1] - TIME[i]))

# PROFILE = DERIVATIVE
# TIME = TIME[:-1]

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