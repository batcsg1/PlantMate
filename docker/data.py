#src: https://www.youtube.com/watch?v=fIlklRIuXoY&t=238s
#Run pip install pyserial 

from serial.tools import list_ports
import serial
import time
import csv
from datetime import datetime

import os

file = "dry.csv"
# ChatGPT code for checking if:
# the file exists and is empty
file_exists = os.path.exists(file)
file_empty = os.path.getsize(file) == 0 if file_exists else True

# Loop through existing COM ports and print them to terminal
ports = list_ports.comports()
for port in ports: print(port)

# Create data file
f = open(file, "a",newline='')


# Target Arduino serial port
serialCom = serial.Serial('COM3', 9600)

# Reset the arduino
serialCom.setDTR(False)
time.sleep(1) # 1 second per reading
serialCom.flushInput() # Clear the serial com
serialCom.setDTR(True)

def isfloat(num):
    try:
        float(num)
        return True
    except ValueError:
        return False

rows = 10 # Number of data rows to record

for row in range(rows):
    try:
        # Read in a line of data
        s_bytes = serialCom.readline()
        # Decode the binary
        decoded_bytes = s_bytes.decode("utf-8").strip('\r\n')
        # Timestamp
        timestamp = datetime.now().isoformat()
        # Time
        time_12_hour = datetime.now().strftime("%I:%M %p")
        
        #Parse lines
        if row == 0 and file_empty: #Target headers
            values = decoded_bytes.split(",")
            values.insert(0, "Timestamp")
            values.insert(1, "Time")
        else: # Target data rows
            values = [float(x) for x in decoded_bytes.split()]

            # Skip non data like readings
            # Source ChatGPT
            # Prompt: how to skip non-data readings
            if len(values) != 3 and not all(isfloat(v) for v in values):
                continue

            values.insert(0, timestamp)
            values.insert(1, time_12_hour)

        print(f"Row: {row + 1} {values}") #Print the data

        #Write data to CSV

        writer = csv.writer(f,delimiter=",")
        writer.writerow(values)
    except:
        print("ERROR, Line was not recorded.")

f.close() #Close CSV file


