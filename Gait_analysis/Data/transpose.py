from emulator import *
from labeler import *
import numpy as np

SUBJECTS = 'AB06'
FEATURES = [np.average,
            np.max,
            np.min,
            np.std,
            np.median,
            lambda x : x[0],
            lambda x : x[-1]]
# FEATURES = [lambda x : x[0],
#             np.average]

MEASURES = ['Header',
            'foot_Accel_X',
            'foot_Accel_Y',
            'foot_Accel_Z',
            'foot_Gyro_X',
            'foot_Gyro_Y',
            'foot_Gyro_Z']
# MEASURES = ['Header',
#             'foot_Accel_X',
#             'foot_Gyro_X']
# MEASURES = ['T',
#             'x',
#             'dx',
#             'd2x']

labels = Labeler(20, r'AB06/fsr.csv').write()
# labels = np.vstack((np.linspace(0, 1, 10000), np.linspace(0, 1, 10000)))
print(labels[0][0], labels[0][-1])
emulator = Emulator(FEATURES, 0.5, labels, r'AB06/imu.csv', MEASURES, r'pdata.csv')
# emulator = Emulator(FEATURES, 0.5, labels, r'func.csv', MEASURES, r'fdata.csv')


emulator.write()
# print(str(len(FEATURES) * (len(MEASURES) - 1)) + " features written")