import numpy as np


def close(x, y, PRECISION):
    return abs(x - y) < PRECISION

class Stepper:

    def __init__(self):
        self.output = []
        self.step0 = 0

    # def train(self, times, percents):
    #     self.step0 = -1 * float('infinity')
    #     for t, p in zip(times, percents):
    #         if close(p, 0, 1):
    #             self.step0 = t
    #             print(t)
    #         # print(p)
                
    #         if self.step0 != -1 * float('infinity') and close(p, 100, 1):
    #             self.step0 = t - self.step0
    #             print(t)
    #             break
    
    # def predict(self, steps, interval=0.005):
    #     ret = []
    #     k = 0
    #     for i in range(0, len(steps) - 1):
    #         start, final = steps[i], steps[i + 1]
    #         t = 0
    #         while t < final - start:
    #             ret.append(t / (final - start))
    #             t += interval
    #             k += 1
    #     return ret

    
    # def percent_to_steps(self, times, percents):
    #     steps = []
    #     first = False
    #     next = False
    #     for t, p in zip(times, percents):
    #         if not first and close(0, p, 1):
    #                 first = True
            
    #         if close(0, p, 1) and next:
    #             steps.append(t + self.step0)
    #             next = False
    #         elif close(100, p, 1):
    #             next = True
        
    #     return steps

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
                


            