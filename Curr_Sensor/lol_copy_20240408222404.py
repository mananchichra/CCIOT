from datetime import datetime
import serial

ser = serial.Serial('/dev/ttyACM0', 9600)  # Adjust COM port accordingly

with open('Cycle5.txt', 'a') as file:
    while True:
        timestamp = datetime.now()
        data = ser.readline().decode().strip()
        line = f"{timestamp}, {data}"

        print(line)
        file.write(line + '\n')

ser.close()
