import serial
DEVICE="/dev/ttyACM0"

print(f"Connecting to {DEVICE}...", end="")
connected = False

while not connected:
    try:
        ser = serial.Serial(DEVICE, 115200)
        connected = True
        print("connected!")
    except serial.serialutil.SerialException:
        pass

try:
    while connected:
        x = ser.read()
        try:
            print(x.decode("utf-8"), end="")
        except UnicodeDecodeError as err:
            print(f"Decode fail: {err}")
except KeyboardInterrupt:
    ser.close()

print("\nDone.")

