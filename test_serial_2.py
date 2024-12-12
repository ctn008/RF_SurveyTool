import serial
import time

# Configure the serial port (adjust as needed)
port = "/dev/ttyUSB0"  # Replace with the correct port (e.g., COM3, COM4)
baud_rate = 115200
# Initialize Serial
try:
    ser = serial.Serial(port, baud_rate)
    print(f"Connected to {port} at {baud_rate} bps")
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit()

# Continuously read data
try:
    while True:
        if ser.in_waiting > 0:
            data = ser.readline().decode('utf-8').strip()
            print(f"Received: {data}")
        time.sleep(0.0001)  # Add a small delay to avoid busy-waiting
except KeyboardInterrupt:
    print("Exiting...")
finally:
    ser.close()
    print("Serial port closed")
