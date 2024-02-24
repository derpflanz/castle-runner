import argparse, os, sys
from fileprocessing import lexer, opcodes
from directives import directives
from assembler import assembler

parser = argparse.ArgumentParser(description='6502 Assembler')
parser.add_argument('inputfile', type=str, help='.asm file to process')
parser.add_argument('outputfile', type=str, help='Binary .hex file to generate')
parser.add_argument('-c', '--opcodefile', type=str, help='Opcode file to use')
parser.add_argument('-d', '--debuginfo', type=str, help='Where to store debug info')
parser.add_argument('-r', '--result', action='store_true', help='Show resulting code')
parser.add_argument('-l', '--show-labels', action='store_true', help='Show generated label addresses')
parser.add_argument('-s', '--starting-address', type=str, default='8000',
    help='Starting address, in HEX (e.g. -s 8000). The address of the first opcode is stored at the beginning of the file. Default is 8000.')
parser.add_argument('-t', '--target', choices=['c64', 'cr1'], default='cr1', help='Type of HEX file. "c64" will put location on where to store the image, "cr1" will set the RESB vector to the first opcode.')

args = parser.parse_args()

if not os.path.exists(args.inputfile):
    print(f"Input file '{args.inputfile}' does not exist.")
    sys.exit(1)

if args.opcodefile is None or not os.path.exists(args.opcodefile):
    print(f"Opcode file {args.opcodefile} not found. Cannot assemble.")
    sys.exit(1)

if os.path.exists(args.outputfile):
    print(f"Output file '{args.outputfile}' already exists, will be overwritten.")

try:
    starting_address = int(args.starting_address, 16)

    if starting_address < 0 or starting_address > 0xffff:
        raise Exception()
except:
    print(f"Address '{args.starting_address}' is not a valid address. Must be hex and between 0x0000 and 0xFFFF")
    sys.exit(1)

print(f"Going to create HEX file for target '{args.target}'")
print(f"Using ${args.starting_address} as starting address, all labels will be calculated from here.")

with open(args.inputfile, "r") as ifile, open(args.outputfile, 'wb') as ofile:    
    opcodes.opcode_init(args.opcodefile)
    mylexer = lexer.AsmLexer()
    directives = directives.Directives(args.debuginfo)

    assembler = assembler.Assembler(mylexer, directives)
    assembler.starting_address = starting_address
    assembler.show_labels = args.show_labels
    assembler.target = args.target
    assembler.show_result = args.result

    assembler.assemble(ifile, ofile)
