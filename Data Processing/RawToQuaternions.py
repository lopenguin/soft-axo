import csv
import string
import pathlib
import math

#----------Change these file names to fit-----------------------------------------------
RAW_DATA_FILE = 'walking.csv'
QUATERNION_SAVE_FILE = 'walking_quaternions.csv'
EULER_SAVE_FILE = 'walking_euler.csv'
#---------------------------------------------------------------------------------------

# sourced from https://automaticaddison.com/how-to-convert-a-quaternion-into-euler-angles-in-python/
def euler_from_quaternion(q):
        """
        Convert a quaternion into euler angles (roll, pitch, yaw)
        roll is rotation around x in radians (counterclockwise)
        pitch is rotation around y in radians (counterclockwise)
        yaw is rotation around z in radians (counterclockwise)
        """
        x = q[0]
        y = q[1]
        z = q[2]
        w = q[3]
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
     
        return roll_x, pitch_y, yaw_z # in radians

# begin script
with open(EULER_SAVE_FILE, mode = 'w') as walking_data:
    with open(QUATERNION_SAVE_FILE, mode = 'w') as quaternion_data:
        with open(RAW_DATA_FILE) as raw_data:
            # just setting up csv reader/writer objects...
            euler_writer = csv.writer(walking_data, delimiter = ',', lineterminator = '\n')
            quaternion_writer = csv.writer(quaternion_data, delimiter = ',', lineterminator = '\n')
            read_data = csv.reader(raw_data, delimiter = ',')
            
            # simple counters
            line_counter = 0
            ignored_line_counter = 0

            for row in read_data:
                quaternion_row = [] # list to hold quaternions (per row)
                for component in row:
                    skip = False
                    # some lines have spaces, just added a double-check if statement
                    # NOTE: THIS WONT WORK IF HEXADECIMALS START WITH 0x
                    if (not all(c in string.hexdigits for c in component)):
                        skip = True
                        ignored_line_counter += 1
                        break
                    decimal = int(component, 16) / 1000.0 - 1
                    quaternion_row.append(decimal)
                if skip: # if skip is true, that means the line was somehow invalid for hexadecimals
                    continue
                line_counter += 1
                
                # quaternion writing below: writing two to a row like before
                quaternion_writer.writerow(list(map(str, quaternion_row)))

                # euler writing below: assumed you had two IMUs -> two quaternions per row
                euler_writer.writerow(list(map(str, euler_from_quaternion(quaternion_row[0:4]) + euler_from_quaternion(quaternion_row[4:8]))))

        # output stuff
        print('---')
        print(f'Processed {line_counter} lines. (Ignored {ignored_line_counter} lines)')
        print(f'Saved quaternion data to {pathlib.Path().resolve()}\{QUATERNION_SAVE_FILE}')
        print(f'Saved euler data to {pathlib.Path().resolve()}\{EULER_SAVE_FILE}')
        print('---')