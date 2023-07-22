import argparse, os, sys
from eeprom import eeprom

parser = argparse.ArgumentParser(description='EEPROM Programmer')
parser.add_argument('action', type=str, help='Action to do', choices=['read','write'])
parser.add_argument('-f', '--file', dest='file', help='Filename of file to write or to store results in')
parser.add_argument('-l', '--length', dest='length', type=int, help='Length of data to read or write (may be omitted when writing a HEX file')
parser.add_argument('-o', '--overwrite', dest='overwrite', help='Overwrite file if already exists.', action='store_true')
parser.add_argument('-p', '--port', dest='port', help='Serial port to use (probably /dev/ttyACM0)', default='/dev/ttyACM0')
parser.add_argument('-s start_address', '--start_address', dest='start_address', help='Address to start reading (in hex, e.g. -s ff00). Default 0. Ignored for writing.', default='0')
args = parser.parse_args()

if args.action == 'read' and os.path.exists(args.file) and args.overwrite == False:
    print(f"Cannot read into {args.file}: file already exists")
    sys.exit(-1)

if args.action == 'write' and not os.path.exists(args.file):
    print(f"Cannot write from {args.file}: file does not exist")
    sys.exit(-1)

if args.action == 'read' and args.length is None:
    print("When reading, you must supply a how many bytes to read (-l, --length)")
    sys.exit(-1)

if args.action == 'read':
    print(f"Going to read {args.length} bytes into {args.file}, using {args.port}")

    eeprom = eeprom.Eeprom(args.port, 9600)
    data = eeprom.read(args.start_address, args.length)
    with open(args.file, "wb") as file:
        file.write(data)        
elif args.action == 'write':
    length = os.path.getsize(args.file)
    if args.length is not None and args.length > length:
        print(f"Given length ({args.length}) is larger than size of file; using {length}")

    if args.length is not None and args.length < length:
        length = args.length

    print(f"Going to write {length} bytes from {args.file}, using {args.port}")

    with open(args.file, "rb") as file:
        data = file.read()
        
    eeprom = eeprom.Eeprom(args.port, 9600)
    eeprom.write(data)

    print(f"Wrote {len(data)} bytes.")


