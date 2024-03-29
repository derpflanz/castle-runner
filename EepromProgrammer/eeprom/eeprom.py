import serial, string, time

SYN   = b'\x16'
SOH   = b'\x01'
ETB   = b'\x17'
STX   = b'\x02'
ETX   = b'\x03'
EOT   = b'\x04'
ACK   = b'\x06'
NAK   = b'\x15'
US    = b'\x1f'
WRITE_CODE = b'\x31'
READ  = b'\x32'
WRITE_DATA = b'\x33'

class Eeprom:
    _port = "/dev/ttyACM0"
    _speed = 9600
    _verbose = True

    def __init__(self, port, speed):
        self._port = port
        self._speed = speed

    def _print(self, message, end='\n'):
        if self._verbose == True:
            print(message, end=end)

    def _send_header(self, ser, command, start_address, length, resb = 0, irq = 0):
        result = False

        # wait a little for the Nano to wake up
        time.sleep(0.5)

        ser.write(SYN)
        head_bytes_received = 0
        while True:
            received = ser.read()
            if received == SYN:
                self._print(f"\n\nReceived {head_bytes_received} header bytes, and SYN, starting command")
                break
            else:
                self._print(received.decode('iso-8859-1'), end='')
                head_bytes_received += 1
                
        self._print(f"""
                    Sending header: 
                    command {command}
                    start_address: {start_address} 
                    length of data: {length} 
                    resb vector: {resb}
                    irq vector: {irq}
        """)

        ser.write(SOH)
        ser.write(command)
        ser.write(str.encode(f"{start_address}"))
        ser.write(US)
        ser.write(str.encode(f"{length}"))
        ser.write(US)
        ser.write(str.encode(f"{resb}"))
        ser.write(US)
        ser.write(str.encode(f"{irq}"))
        ser.write(ETB)

        received = ser.read()
        if received == ACK:
            self._print("Got ACK, header was received correctly.")
            result = True
        elif received == NAK:
            self._print("Got NAK, some problem with header.")
        else:
            self._print(f"ERROR, received unexpected byte {received}")

        return result

    def write(self, start_address, _bytes, _type, verbose = True):
        self._verbose = verbose
        self._print(f"Connecting to {self._port} with {self._speed} baud. Reset the reader if this blocks.")

        with serial.Serial(self._port, self._speed) as ser:
            resb = "0000"
            irq = "0000"
            start_offset_in_file = 0

            if _type == WRITE_CODE:
                # resb is stored little endian in code file, we need it as an ascii
                # hex address for the programmer
                resb = f"{_bytes[1]:02x}{_bytes[0]:02x}"
                irq =  f"{_bytes[3]:02x}{_bytes[2]:02x}"
                start_offset_in_file = 4

            if self._send_header(ser, _type, start_address, len(_bytes) - start_offset_in_file, resb, irq):
                self._print("Header sent, continuing with sending data.")
                
                ser.write(STX)

                self._print("")
                self._print(f"{start_address} ", end='')
                self._print_ctr = int(start_address, 16)
                for b in _bytes[start_offset_in_file:]:
                    ser.write(bytes([b]))

                    byte_read = ser.read()
                    self._printByte(byte_read)

                ser.write(ETX)

                if ser.read() == EOT:
                    self._print("\n\nProgrammer received all data. Done.")

    _print_ctr = 0
    _print_line = b''
    _print_spc = None
    def _printByte(self, b):
        if self._print_spc is None:
            self._print_spc = self._print_ctr

        filter = ''.join([['.', chr(x)][chr(x) in string.printable[:-5]] for x in range(256)])

        if isinstance(b, int):
            b = bytes([b])

        self._print_line += b
        self._print(f"{b.hex()} ", end='')

        self._print_ctr += 1
        if self._print_ctr % 16 == 0:
            self._print(f"[{' '*(self._print_spc%16)}{self._print_line.decode('iso-8859-1').translate(filter)}]\n{self._print_ctr:04x} ", end='')
            self._print_line = b''
            self._print_spc = 0

    def read(self, start_address, length, verbose = True):
        self._verbose = verbose
        self._print_ctr = int(start_address, 16)
        self._print(f"Connecting to {self._port} with {self._speed} baud. Reset the reader if this blocks.")

        received_data = b''

        with serial.Serial(self._port, self._speed) as ser:
            if self._send_header(ser, READ, start_address, length):
                self._print("Header sent, continuing with reading data.")
                received = ser.read()

                recv_bytes = 0
                if received == STX:
                    self._print("Got STX, receiving data...")
                    spc = self._print_ctr % 16
                    self._print(f"{self._print_ctr:04x} {' '*(spc*3)}", end='')
                    while recv_bytes < length:
                        received = ser.read()
                        received_data += received
                        self._printByte(received)
                        recv_bytes += 1

                    if ser.read() == ETX:
                        self._print(f"\nGot {recv_bytes} bytes and ETX, we're done!")
                    else:
                        self._print(f"\nGot {recv_bytes} bytes but no ETX!")
                
                received = ser.read()
                if received == EOT:
                    self._print("Got EOT, close the connection.")

        return received_data