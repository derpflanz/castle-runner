import argparse, os, sys
from fileprocessing import lexer, opcodes
import sly

parser = argparse.ArgumentParser(description='6502 Assembler')
parser.add_argument('inputfile', type=str, help='.asm file to process')
parser.add_argument('outputfile', type=str, help='Binary .hex file to generate')
parser.add_argument('-s', '--starting-address', type=str, default='8000',
    help='Starting address, in HEX (e.g. -s 8000). This address is stored at the beginning of the file. Default is 8000.')

args = parser.parse_args()

if not os.path.exists(args.inputfile):
    print(f"Input file '{args.inputfile}' does not exist.")
    sys.exit(1)

if os.path.exists(args.outputfile):
    print(f"Output file '{args.outputfile}' already exists, will be overwritten.")

try:
    starting_address = int(args.starting_address, 16)

    if starting_address < 0 or starting_address > 0xffff:
        raise Exception()
except:
    print(f"Address '{args.starting_address}' is not a valid address.")
    sys.exit(1)

print(f"Using ${args.starting_address} as starting address")

with open(args.inputfile, 'r') as ifile:
    # read the lines of the file into an array, so we can safely edit them
    lines = []
    for line in ifile:
        lines.append(line)

with open(args.outputfile, 'wb') as ofile:
    ofile.write(starting_address.to_bytes(2, 'little'))

    mylexer = lexer.AsmLexer()
    lineno = 1
    address = starting_address
    line = ''
    labels = {}

    try:
        # first process labels and string names
        for line in lines:
            result = list(mylexer.tokenize(line))
            codes = opcodes.Opcodes(result)

            if len(result) > 0 and (result[0].type == lexer.TOK_LABEL or result[0].type == lexer.TOK_STRINGNAME):
                labels[result[0].value] = f"{address:04x}"

            address += codes.length()

        print(f"Labels: {labels}")

        # then preprocess the lines, replacing all labels and string names with their absolute address
        # this way, the assembler only needs to handle absolute addresses
        preprocessed_lines = []
        for line in lines:
            line = line.strip()
            for label,address in labels.items():
                if line.startswith(label):
                    line = line.strip(label)

                line = line.replace(label, '$' + address)

            preprocessed_lines.append(line)

        address = starting_address

        for line in preprocessed_lines:
            result = list(mylexer.tokenize(line))
            codes = opcodes.Opcodes(result, address)
            ofile.write(codes.as_bytes())

            a = "    " if codes.length() == 0 else f"{address:04x}"
            s = f"[{lineno:5}:{a}] {line.strip()}"
            h = codes.as_bytes().hex(' ', 1)
            print(f"{s:80} {h}")
            
            lineno += 1
            address += codes.length()

    except opcodes.OpcodeError as err:
        print(f"ERROR: {err}")
        print(f"Hex file {args.outputfile} was not written correctly.")
        sys.exit(1)
    except SyntaxError as err:
        print(f"ERROR: {err}")
        print(f"Hex file {args.outputfile} was not written correctly.")
        sys.exit(1)
    except sly.lex.LexError as err:
        print(f"[{lineno:5}:    ] {line.strip()}")
        print(f"ERROR: {err}")
        print(f"Hex file {args.outputfile} was not written correctly.")
        sys.exit(1)

            
