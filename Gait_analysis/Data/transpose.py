from emulator import *
from labeler import *
import numpy as np

SUBJECTS = 'AB06'
FEATURES = [np.average,
                   np.max,
                   np.min,
                   lambda x : x[0],
                   lambda x : x[-1]]

MEASURES = ['Header',
            'foot_Gyro_X',
            'foot_Gyro_Y',
            'foot_Gyro_Z']

labels = Labeler(20, r'AB06/fsr.csv').write()
emulator = Emulator(FEATURES, 2, labels, r'AB06/imu.csv', MEASURES, r'pdata.csv')

emulator.write()