import serial, sys, os

ENQ = b'\x05'
STX = b'\x02'
ETX = b'\x03'
NAK = b'\x15'
ACK = b'\x06'
EOT = b'\x04'

if len(sys.argv) < 3:
    print(f"Usage: {sys.argv[0]} <hexfile> <tty>")
    sys.exit(1) 

hexfile = sys.argv[1]
filesize = os.path.getsize(hexfile)

print(f"Going to send {hexfile}: {filesize} bytes over {sys.argv[2]}.")

with serial.Serial(sys.argv[2], 2400) as ser:
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

            ctr = 0
            with open(hexfile, "rb") as file:            
                while (byte := file.read(1)):
                    ser.write(byte)
                    
#                    ctr += 1
#                    perc = (ctr / filesize) * 100
#                    if ctr % 1000 == 0:
#                        print(f"{ctr} bytes written ({round(perc, 2)}%)")
                
            ser.write(EOT)
            print("Done sending data.")
            sending = False
        elif received == EOT:
            sending = False
        else:
            print(received.decode(), end='')

        # do I have stuff to send?
            