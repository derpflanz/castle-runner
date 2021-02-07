import argparse, os, sys
from fileprocessing import lexer, opcodes

parser = argparse.ArgumentParser(description='6502 Assembler')
parser.add_argument('inputfile', type=str, help='.asm file to process')
parser.add_argument('outputfile', type=str, help='Binary .hex file to generate')

args = parser.parse_args()

if not os.path.exists(args.inputfile):
    print(f"Input file '{args.inputfile}' does not exist.")
    sys.exit(-1)

if os.path.exists(args.outputfile):
    print(f"Output file '{args.outputfile}' already exists, will be overwritten.'")

with open(args.inputfile, 'r') as ifile:
    with open(args.outputfile, 'wb') as ofile:
        lexer = lexer.AsmLexer()
        lineno = 0

        try:
            for line in ifile:
                print(f"[{lineno:5}] {line.strip()}")
                result = lexer.tokenize(line)
                codes = opcodes.Opcodes(result)
                codes.process()
                
                ofile.write(codes.as_bytes())

                lineno += 1
        except opcodes.OpcodeError as err:
            print(f"ERROR: {err}")
            print(f"Hex file {args.outputfile} was not written correctly.")
