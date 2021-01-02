import serial, sys, os

ENQ = b'\x05'
STX = b'\x02'
ETX = b'\x03'
NAK = b'\x15'
ACK = b'\x06'
EOT = b'\x04'

if len(sys.argv) == 1:
    print(f"Usage: {sys.argv[0]} <hexfile>")
    sys.exit(1) 

hexfile = sys.argv[1]
filesize = os.path.getsize(hexfile)

print(f"Going to send {hexfile}: {filesize} bytes.")

ser = serial.Serial("/dev/ttyACM2")

with serial.Serial("/dev/ttyACM2", 9600) as ser:
    sending = True

    while sending:
        received = ser.read()
        
        if received == ENQ:
            print("Party said they are ready to receive, sending header")
            ser.write(STX)
            ser.write(str.encode(f"{filesize}"))
            ser.write(ETX)
        elif received == NAK:
            print("Party is in error, stop.");
            sending = False
        elif received == ACK:
            print("Party is ready to receive data, sending data")
            with open(hexfile, "rb") as file:            
                while (byte := file.read(1)):
                    ser.write(byte)
                
            ser.write(EOT)
            print("Done sending data.")
            
            sending = False
        else:
            print(received.decode(), end='')

        # do I have stuff to send?
            