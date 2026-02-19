import serial
import os

# Ensure the directory exists
os.makedirs('uart_logs', exist_ok=True)

# Open the serial port
ser = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=1)

# Open the log file for writing
with open('uart_logs/logs.txt', 'w') as log_file:
    try:
        while True:
            # Read a line from the serial port
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            
            if line:
                print(line)
                log_file.write(line + '\n')
                log_file.flush()
                
                # Check for the stop condition
                if 'finished encoding' in line.lower():
                    print("Stop condition detected. Exiting...")
                    break
    except KeyboardInterrupt:
        print("Interrupted by user")
    finally:
        ser.close()