import argparse, os, sys
from eeprom import eeprom

PORT_SPEED = 57600

parser = argparse.ArgumentParser(description='EEPROM Programmer')
parser.add_argument('action', type=str, help='Action to do', choices=['read','write-code', 'write-data', 'verify'])
parser.add_argument('-f', '--file', dest='file', help='Filename of file to write to EEPROM, read contents into or verify against (HEX)')
parser.add_argument('-l', '--length', dest='length', type=int, help='Length of data to read or write (may be omitted when writing or verifying a HEX file')
parser.add_argument('-o', '--overwrite', dest='overwrite', help='Overwrite file if already exists.', action='store_true')
parser.add_argument('-p', '--port', dest='port', help='Serial port to use (probably /dev/ttyACM0)', default='/dev/ttyACM0')
parser.add_argument('-s start_address', '--start_address', dest='start_address', help='Address to start reading (in hex, e.g. -s ff00). Default 0000.', default='0000')
args = parser.parse_args()

if args.action == 'verify' and args.file is None:
    print(f"Need a file (hex) to verify against.")
    sys.exit(-1)

if args.action == 'verify' and not os.path.exists(args.file):
    print(f"Cannot read {args.file} for verifying.")
    sys.exit(-1)

if args.action == 'read' and os.path.exists(args.file) and args.overwrite == False:
    print(f"Cannot read into {args.file}: file already exists")
    sys.exit(-1)

if (args.action == 'write-code' or args.action == 'write-data') and not os.path.exists(args.file):
    print(f"Cannot write from {args.file}: file does not exist")
    sys.exit(-1)

if args.action == 'read' and args.length is None:
    print("When reading, you must supply a how many bytes to read (-l, --length)")
    sys.exit(-1)

if args.action == 'read':
    print(f"Going to read {args.length} bytes into {args.file}, using {args.port}")

    eeprom_reader = eeprom.Eeprom(args.port, PORT_SPEED)
    data = eeprom_reader.read(args.start_address, args.length)
    with open(args.file, "wb") as file:
        file.write(data)
elif args.action == 'verify':
    with open(args.file, "rb") as f:
        f.seek(4)           # skip the RES and IRQ vectors
        file_bytes = f.read()

    length = len(file_bytes)
    print(f"Checking first {length} bytes of EEPROM against {args.file}")

    eeprom_reader = eeprom.Eeprom(args.port, PORT_SPEED)
    data = eeprom_reader.read(args.start_address, length, verbose = False)

    i = 0
    errors = 0
    while i < length:
        if file_bytes[i] != data[i]:
            print(f"Mismatch at offset: {i:04x}: expected {file_bytes[i]:02x}, read {data[i]:02x}")
            errors += 1
        i += 1

    print(f"Found {errors} error(s).")

elif args.action == 'write-code' or args.action == 'write-data':
    length = os.path.getsize(args.file)
    if args.length is not None and args.length > length:
        print(f"Given length ({args.length}) is larger than size of file; using {length}")

    if args.length is not None and args.length < length:
        length = args.length

    print(f"Going to {args.action} {length} bytes from {args.file}, using {args.port}")

    with open(args.file, "rb") as file:
        data = file.read()
        
    type = eeprom.WRITE_CODE
    if (args.action == "write-data"):
        type = eeprom.WRITE_DATA

    eeprom_writer = eeprom.Eeprom(args.port, PORT_SPEED)
    eeprom_writer.write(args.start_address, data, type)

    print(f"Wrote {len(data)} bytes ({len(data) - 4} data, 4 bytes vectors)")


