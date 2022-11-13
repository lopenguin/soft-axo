import platform, csv

SUBJECTS = ['AB06']
ENTRYS = ['Header'] + ['foot_Gyro_X', 'foot_Gyro_Y', 'foot_Gyro_Z']
LABEL = True
WRITE_LOCATION = ''
WRITE_NAME = 'data.csv'
if platform.system() == 'Windows':
    WRITE_LOCATION = r'..\\Neural_net\\'
else:
    WRITE_LOCATION = r'../Neural_net/'


def fsr2steps(labels):
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
            if labels[1][i] > 20:
                stepped = False
                steps.append(min_t)
                min_t, min_x = 99999, 99999
    return steps


def steps2percentage(steps):
    ret = [[], []]
    for i in range(0, len(steps) - 1):
        start, final = steps[i], steps[i + 1]
        t = 0
        while t < final - start:
            ret[1].append(t / (final - start))
            ret[0].append(t + start)
            t += 0.005
    return ret



with open(WRITE_LOCATION + WRITE_NAME, 'w') as wfile:
    writer = csv.writer(wfile, lineterminator='\n')
    for subject in SUBJECTS:
        if LABEL:
            labels = [[], []]
            with open(subject + '/fsr.csv') as ffile:
                reader = csv.reader(ffile)
                for row in reader:
                    if row[0] == 'Header': continue
                    labels[0].append(float(row[0]))
                    labels[1].append(float(row[1]))
            steps = fsr2steps(labels)
            gait_labels = steps2percentage(steps)

                    
        with open(subject + '/imu.csv') as rfile:
            reader = csv.reader(rfile)
            idx = []
            gait_idx = 0
            for row in reader:
                if row[0] == 'Header':
                    for i, elem in enumerate(row):
                        if elem in ENTRYS:
                            idx.append(i)
                else:
                    towrite = [0] * len(idx)
                    index = 0
                    for i in idx:
                        towrite[index] = float(row[i])
                        index += 1
                    if gait_labels != None:
                        if gait_idx < len(gait_labels[0]) and gait_labels[0][gait_idx] > towrite[0]:
                            towrite.append(gait_labels[1][gait_idx])
                            gait_idx += 1
                    writer.writerow(towrite)



