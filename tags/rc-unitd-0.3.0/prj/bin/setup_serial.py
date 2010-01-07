# serial_test.py
#
#   use pySerial package to setup bbox serial
#
#   read lines from the Arduino on decv/ttyUSB0
################################################

import serial

ser = serial.Serial('/dev/ttyUSB0', 57600)
    
ser.close()             #close port from import serial

print	"\nButton Box Serial Setup Complete\n"

