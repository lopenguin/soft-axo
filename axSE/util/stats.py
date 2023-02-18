from matplotlib import pyplot as plt
import numpy as np

def get_stats(y_pred, y, tols=[1, 2, 3, 4, 5, 10, 20], max=100):
    error = []
    wrong = [0] * len(tols)
    for y, yp in zip(y, y_pred):
        error.append([y, min(abs(y - yp), max - abs(y - yp))])
        for k, t in enumerate(tols):
            if min(abs(y - yp), abs(max - abs(yp - y))) >= t:
                wrong[k] += 1

    for w, t in zip(wrong, tols):
        print('NN is within ' + str(t) + r'% of the time', 1 - w / len(y_pred))

    error = np.array(error, dtype=list)
    plt.scatter(error[:,0], error[:,1], s=1)
    plt.show()