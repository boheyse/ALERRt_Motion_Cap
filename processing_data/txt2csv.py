# import csv
#
# with open('/Users/Heyseb1/Documents/D2/processing_data/$cenario5/Scenario5_Chest.txt', 'r') as in_file:
#     stripped = (line.strip() for line in in_file)
#     lines = (line.split(",") for line in stripped if line)
#     with open('log.csv', 'w') as out_file:
#
#         writer = csv.writer(out_file)
#         writer.writerow(('Timestamp', 'Gyroscope X (deg/s)', 'Gyroscope Y (deg/s)', 'Gyroscope Z (deg/s)',
#                         'Accelerometer X (g)', 'Accelerometer Y (g)', 'Accelerometer Z (g)',
#                         'Magnetometer X (G)', 'Magnetometer Y (G)', 'Magnetometer Z (G)'))
#         writer.writerows(lines)

import pandas as pd
import re

data = pd.read_csv('/Users/Heyseb1/Documents/D2/processing_data/$cenario6/Scenario6_Chest.txt', sep=",", header=None)
data.columns = ['Timestamp', 'Gyroscope X (deg/s)', 'Gyroscope Y (deg/s)', 'Gyroscope Z (deg/s)',
                         'Accelerometer X (g)', 'Accelerometer Y (g)', 'Accelerometer Z (g)',
                         'Magnetometer X (G)', 'Magnetometer Y (G)', 'Magnetometer Z (G)']
data.to_csv('f.csv', index=False)
