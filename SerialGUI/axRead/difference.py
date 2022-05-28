"""
Processes quaternion -> euler data and interprets dorsiflexion angle
"""

import math, csv
import numpy as np
 
def q2e(q):
        """
        Convert a quaternion into euler angles (roll, pitch, yaw)
        roll is rotation around x in radians (counterclockwise)
        pitch is rotation around y in radians (counterclockwise)
        yaw is rotation around z in radians (counterclockwise)
        """
        w, x, y, z = q[0], q[1], q[2], q[3]
        t0 = +2.0 * (w * x + y * z)
        t1 = +1.0 - 2.0 * (x * x + y * y)
        roll_x = math.atan2(t0, t1)
     
        t2 = +2.0 * (w * y - z * x)
        t2 = +1.0 if t2 > +1.0 else t2
        t2 = -1.0 if t2 < -1.0 else t2
        pitch_y = math.asin(t2)
     
        t3 = +2.0 * (w * z + x * y)
        t4 = +1.0 - 2.0 * (y * y + z * z)
        yaw_z = math.atan2(t3, t4)
     
        return np.array([roll_x, pitch_y, yaw_z]) # in radians

IMU_FILE = r"data_I .csv"

QUATERNION_FILE = r"q.csv"
EULER_FILE = r"e.csv"

with open(IMU_FILE) as ifile:
    with open(QUATERNION_FILE, 'w') as qfile:
        with open(EULER_FILE, 'w') as efile:

            ireader = csv.reader(ifile, delimiter=',')
            qwriter = csv.writer(qfile, delimiter=',',lineterminator='\n')
            ewriter = csv.writer(efile, delimiter=',',lineterminator='\n')

            for row in ireader:
                try:
                    q1 = np.array(row[9:13]).astype(float)
                    q2 = np.array(row[13:17]).astype(float)
                except:
                    print(row)
                assert(len(q1) == len(q2))
                q2conj = [q2[0], -q2[1], -q2[2], -q2[3]]
                q_diff = q2conj*q1
                e_diff = q2e(q1) - q2e(q2)
                qwriter.writerow(np.concatenate(([float(row[0])], q_diff)))
                ewriter.writerow(np.concatenate(([float(row[0])], e_diff)))
