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

# MEASURES = ['Header',
#             'pelvis_tilt',
#             'pelvis_list',
#             'pelvis_rotation',
#             'pelvis_tx',
#             'pelvis_ty',
#             'pelvis_tz',
#             'hip_flexion_r',
#             'hip_adduction_r',
#             'hip_rotation_r',
#             'knee_angle_r',
#             'ankle_angle_r',
#             'subtalar_angle_r',
#             'mtp_angle_r',
#             'hip_flexion_l',
#             'hip_adduction_l',
#             'hip_rotation_l',
#             'knee_angle_l',
#             'ankle_angle_l',
#             'subtalar_angle_l',
#             'mtp_angle_l']
MEASURES = ['Header',
            'ankle_angle_r',
            'subtalar_angle_r',
            'mtp_angle_r']

# MEASURES = ['Header',
#             'foot_Accel_X',
#             'foot_Gyro_X']
# MEASURES = ['T',
#             'x',
#             'dx',
#             'd2x']

labels = Labeler(20, r'AB06/fsr.csv').real_write()
# labels = np.vstack((np.linspace(0, 1, 10000), np.linspace(0, 1, 10000)))
print(labels[0][0], labels[0][-1])
emulator = Emulator(FEATURES, 1, labels, r'AB06/ik.csv', MEASURES, r'pdata.csv')
# emulator = Emulator(FEATURES, 0.5, labels, r'func.csv', MEASURES, r'fdata.csv')


emulator.write()
# print(str(len(FEATURES) * (len(MEASURES) - 1)) + " features written")