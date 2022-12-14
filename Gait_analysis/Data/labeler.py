import csv

class Labeler:

    def __init__(self, threshold, file_in, file_out=None):
        self.file_in = file_in
        self.file_out = file_out
        self.threshold = threshold

    
    def fsr2steps(self, labels):
        steps = []
        stepped = False
        min_t, min_x = 99999, 99999
        for i, elem in enumerate(labels[0]):
            if not stepped and labels[1][i] < 20:
                stepped = True
            elif stepped:
                if labels[1][i] < min_x:
                    min_t = labels[0][i]
                    min_x = labels[1][i]
                if labels[1][i] > self.threshold:
                    stepped = False
                    steps.append(min_t)
                    min_t, min_x = 99999, 99999
        return steps


    def steps2percentage(self, steps):
        ret = [[], []]
        for i in range(0, len(steps) - 1):
            start, final = steps[i], steps[i + 1]
            t = 0
            while t < final - start:
                ret[1].append(t / (final - start))
                ret[0].append(t + start)
                t += 0.005
        return ret

    def write(self):
            labels = [[], []]
            with open(self.file_in) as ffile:
                reader = csv.reader(ffile)
                for row in reader:
                    if row[0] == 'Header': continue
                    labels[0].append(float(row[0]))
                    labels[1].append(float(row[1]))
            steps = self.fsr2steps(labels)
            gait_labels = self.steps2percentage(steps)

            if self.file_out != None:
                with open(self.file_out, 'w') as wfile:
                    writer = csv.writer(wfile, lineterminator='\n')
                    for g in gait_labels:
                        writer.writerow([g])

            return gait_labels