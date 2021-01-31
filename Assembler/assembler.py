import argparse, os, sys
from fileprocessing import lexer, opcodes

parser = argparse.ArgumentParser(description='6502 Assembler')
parser.add_argument('inputfile', type=str, help='.asm file to process')

args = parser.parse_args()

if not os.path.exists(args.inputfile):
    print(f"Input file '{args.inputfile}' does not exist.")
    sys.exit(-1)

with open(args.inputfile, 'r') as file:
    lexer = lexer.AsmLexer()

    for line in file:
        print(f"Processing line: {line}")

        result = lexer.tokenize(line)
        #opcodes = opcodes.Opcodes(result)
        
        for token in result:
            print(f"type={token.type}, value={token.value}")