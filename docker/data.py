from flask import Flask, Response
from serial.tools import list_ports
import serial
import time
import csv
from datetime import datetime
import os

app = Flask(__name__)

@app.route('/')
def data():
    def generate():
        file = "dry.csv"
        # Check if the file exists and is empty
        file_exists = os.path.exists(file)
        file_empty = os.path.getsize(file) == 0 if file_exists else True

        # Open data file for writing
        with open(file, "a", newline='') as f:
            serialCom = serial.Serial('/dev/ttyS0', 9600)

            # Reset the Arduino
            serialCom.setDTR(False)
            time.sleep(1)  # 1 second per reading
            serialCom.flushInput()  # Clear the serial com
            serialCom.setDTR(True)

            def isfloat(num):
                try:
                    float(num)
                    return True
                except ValueError:
                    return False

            rows = 10  # Number of data rows to record
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

                    # Parse lines
                    if row == 0 and file_empty:  # Target headers
                        values = decoded_bytes.split(",")
                        values.insert(0, "Timestamp")
                        values.insert(1, "Time")
                    else:  # Target data rows
                        values = [float(x) for x in decoded_bytes.split()]

                        # Skip non-data readings
                        if len(values) != 3 and not all(isfloat(v) for v in values):
                            continue

                        values.insert(0, timestamp)
                        values.insert(1, time_12_hour)

                    #print(f"Row: {row + 1} {values}")  # Print the data

                    # Write data to CSV
                    writer = csv.writer(f, delimiter=",")
                    writer.writerow(values)

                    # Yield the values to stream to the client
                    yield f"data: {values}\n"

                except Exception as e:
                    print(f"ERROR, Line was not recorded: {e}")
        
        # Once all data is processed, return a final message
        yield 'data: Data collection and CSV writing completed.\n\n'

    return Response(generate(), content_type='text/event-stream')

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=2025, debug=True)
