import argparse, os, sys
from fileprocessing import lexer, opcodes
from directives import directives
import sly

parser = argparse.ArgumentParser(description='6502 Assembler')
parser.add_argument('inputfile', type=str, help='.asm file to process')
parser.add_argument('outputfile', type=str, help='Binary .hex file to generate')
parser.add_argument('-c', '--opcodefile', type=str, help='Opcode file to use')
parser.add_argument('-d', '--debuginfo', type=str, help='Where to store debug info')
parser.add_argument('-s', '--starting-address', type=str, default='8000',
    help='Starting address, in HEX (e.g. -s 8000). This address is stored at the beginning of the file. Default is 8000.')
parser.add_argument('-t', '--target', choices=['c64', 'cr1'], default='cr1', help='Type of HEX file. "c64" will put location on where to store the image, "cr1" will set the RESB vector to the first opcode.')

args = parser.parse_args()

if not os.path.exists(args.inputfile):
    print(f"Input file '{args.inputfile}' does not exist.")
    sys.exit(1)

if args.opcodefile is None or not os.path.exists(args.opcodefile):
    print(f"Opcode file {args.opcodefile} not found. Cannot assemble.")
    sys.exit(1)
else:
    opcodes.opcode_init(args.opcodefile)

if os.path.exists(args.outputfile):
    print(f"Output file '{args.outputfile}' already exists, will be overwritten.")

try:
    starting_address = int(args.starting_address, 16)

    if starting_address < 0 or starting_address > 0xffff:
        raise Exception()
except:
    print(f"Address '{args.starting_address}' is not a valid address.")
    sys.exit(1)

print(f"Going to create HEX file for target '{args.target}'")
print(f"Using ${args.starting_address} as starting address")

with open(args.inputfile, 'r') as ifile:
    # read the lines of the file into an array, so we can safely edit them
    lines = []
    for line in ifile:
        lines.append(line)

with open(args.outputfile, 'wb') as ofile:
    mylexer = lexer.AsmLexer()
    directives = directives.Directives(args.debuginfo)
    lineno = 1
    address = starting_address
    line = ''
    labels = {}
    linenumber = 1

    try:
        first_opcode_address = None

        # first process labels and string names
        for line in lines:
            result = list(mylexer.tokenize(line))
            codes = opcodes.Opcodes(result)

            if len(result) > 0 and (result[0].type == lexer.TOK_LABEL or result[0].type == lexer.TOK_STRINGNAME):
                labels[result[0].value] = f"{address:04x}"

            if len(result) > 0 and result[0].type == lexer.TOK_OPCODE and first_opcode_address == None:
                first_opcode_address = address

            if len(result) > 0 and result[0].type == lexer.TOK_DIRECTIVE:
                directives.Process(result[0].value, address)

            address += codes.length()

            linenumber += 1

        if first_opcode_address == None:
            raise SyntaxError('No opcodes found. This ASM file is useless.')

        # then preprocess the lines, replacing all labels and string names with their hex operand
        # this way, the assembler only needs to handle hex operands
        preprocessed_lines = []
        linenumber = 1
        for line in lines:
            line = line.strip()
            for label,address in labels.items():
                if line.startswith(label):
                    line = line.strip(label)

                line = line.replace(label, '$' + address)

            preprocessed_lines.append(line)

            linenumber += 1

        address = starting_address

        if args.target == 'c64':
            ofile.write(starting_address.to_bytes(2, 'little'))
        else:
            ofile.write(first_opcode_address.to_bytes(2, 'little'))

        linenumber = 1
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

            linenumber += 1

    except opcodes.OpcodeError as err:
        print(f"ERROR: {err} in line {linenumber}")
        print(f"Hex file {args.outputfile} was not written correctly.")
        sys.exit(1)
    except SyntaxError as err:
        print(f"ERROR: {err}")
        print(f"Hex file {args.outputfile} was not written correctly.")
        sys.exit(1)
    except sly.lex.LexError as err:
        print(f"[{linenumber:5}:    ] {line.strip()}")
        print(f"ERROR: {err}")
        print(f"Hex file {args.outputfile} was not written correctly.")
        sys.exit(1)
    finally:
        directives.Finalise()

            
