import argparse, os, sys
from fileprocessing import lexer, opcodes
import sly

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
        mylexer = lexer.AsmLexer()

        try:            
            lineno = 1
            address = 0x8000
            line = ''

            for line in ifile:
                result = list(mylexer.tokenize(line))
                codes = opcodes.Opcodes(result)

                if len(result) > 0:
                    if result[0].type == lexer.TOK_LABEL:
                        print(f"label {result[0].value} is at address {address:04x}")

                address += codes.length()

            lineno = 1
            address = 0x8000
            line = ''
            ifile.seek(0)
            for line in ifile:
                result = list(mylexer.tokenize(line))
                codes = opcodes.Opcodes(result)
                ofile.write(codes.as_bytes())

                if codes.length() == 0:
                    print(f"[{lineno:5}:    ] {line.strip()}")
                else:
                    print(f"[{lineno:5}:{address:04x}] {line.strip()}")
                lineno += 1
                address += codes.length()

        except opcodes.OpcodeError as err:
            print(f"ERROR: {err}")
            print(f"Hex file {args.outputfile} was not written correctly.")
        except SyntaxError as err:
            print(f"ERROR: {err}")
            print(f"Hex file {args.outputfile} was not written correctly.")
        except sly.lex.LexError as err:
            print(f"[{lineno:5}:    ] {line.strip()}")
            print(f"ERROR: {err}")
            print(f"Hex file {args.outputfile} was not written correctly.")

            
