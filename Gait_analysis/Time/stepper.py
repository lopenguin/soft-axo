import numpy as np


def close(x, y, PRECISION):
    return abs(x - y) < PRECISION

class Stepper:

    def __init__(self):
        self.output = []
        self.step0 = 0

    def spin(self, times, percents):
        ts = []
        ret = []
        last = -1 * float('inf')
        added = False
        start = -1
        for i, (t, p) in enumerate(zip(times, percents)):
            if len(ts) == 0:
                if last == -1 * float('inf') and not close(p, 0, 1):
                    continue
                elif close(p, 0, 1):
                    last = t
                elif close(p, 100, 1):
                    ts.append(t - last)
                    start = i + 1
            else:
                if close(p, 0, 1):
                    last = t
                    added = False
                elif not added and close(p, 100, 1):
                    ts.append(t - last)
                    added = True
                
                ret.append((t - last) / np.average(ts[-3:]) * 100)

        return start, ret
            