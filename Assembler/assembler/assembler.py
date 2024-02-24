from fileprocessing import opcodes, lexer
import sly, re

class Assembler:
    def __init__(self, _lexer, _directives):
        self.lexer = _lexer
        self.directives = _directives

        self.starting_address = 0
        self.show_labels = False
        self.show_result = False
        self.labels = {}
        self.variables = {}
        self.phase = "Initialising"
        self.first_opcode_address = None
        self.irq_address = None
        self.current_line = ""
        self.linenumber = 1
        self.target = "cr1"

    def apply_variables(self, line, variables):
        for varname, value in variables.items():
            line = line.replace(varname, value)

        return line

    def print_labels(self):
        for label,address in self.labels.items():
            print(f"{label:30}: {address}")

    def lexing(self, lines):
        self.phase = "Lexing"

        address = self.starting_address

        # first process labels and string names
        for line in lines:
            self.current_line = line
            line = self.apply_variables(line, self.variables)
            result = list(self.lexer.tokenize(line))
            codes = opcodes.Opcodes(result)

            if len(result) > 0 and (result[0].type == lexer.TOK_LABEL or result[0].type == lexer.TOK_STRINGNAME):
                self.labels[result[0].value] = f"{address:04x}"

            if len(result) > 0 and result[0].type == lexer.TOK_OPCODE and self.first_opcode_address == None:
                self.first_opcode_address = address

            if len(result) > 0 and result[0].type == lexer.TOK_DIRECTIVE:
                self.directives.Process(result[0].value, address)

            address += codes.length()

            self.linenumber += 1

        self.irq_address = int(self.labels[':HW_IRQ'], 16)


    def check_lexing_result(self):
        if self.first_opcode_address == None:
            raise SyntaxError('No opcodes found. This ASM file is useless.')

        if ":HW_IRQ" not in self.labels:
            raise SyntaxError("Assembly had no interrupt handler and no default implemented. Stop.")
        
        print(f"Using ${self.first_opcode_address:04x} as the reset vector (first opcode).")
        print(f"Using ${self.irq_address:04x} as the interrupt vector.")

    def preprocessing(self, lines):
        self.phase = 'Preprocessing'

        # then preprocess the lines, replacing all labels and string names with their hex operand
        # this way, the assembler only needs to handle hex operands
        preprocessed_lines = []
        linenumber = 1
        for line in lines:   
            self.current_line = line         
            line = line.strip()
            for label,address in self.labels.items():
                if line.startswith(label):
                    line = line.strip(label)

                # FIXME: when 'label' exists inside another label, this goes wrong
                # the replace should be "whole word"
                line = line.replace(label, '$' + address)

            preprocessed_lines.append(line)

            linenumber += 1

        return preprocessed_lines

    def readlines(self, ifile):
        # read the lines of the file into an array, so we can safely edit them
        lines = []
        for line in ifile:
            lines.append(line)

        return lines

    def write_vectors(self, ofile):
        if self.target == 'c64':
            ofile.write(self.starting_address.to_bytes(2, 'little'))
        else:
            ofile.write(self.first_opcode_address.to_bytes(2, 'little'))
            ofile.write(self.irq_address.to_bytes(2, 'little'))

    def assembling(self, preprocessed_lines, ofile):
        self.phase = "Assembling"

        self.linenumber = 1
        address = self.starting_address
        for line in preprocessed_lines:
            self.current_line = line
            line = self.apply_variables(line, self.variables)
            result = list(self.lexer.tokenize(line))

            codes = opcodes.Opcodes(result, address)
            ofile.write(codes.as_bytes())

            if self.show_result:
                a = "    " if codes.length() == 0 else f"{address:04x}"
                s = f"[{self.linenumber:5}:{a}] {line.strip()}"
                h = codes.as_bytes().hex(' ', 1)
                print(f"{s:80} {h}")
            
            address += codes.length()

            self.linenumber += 1

    def read_variables(self, lines):
        self.phase = "Reading variables"
        pattern = r"(?P<varname>[a-zA-Z_][a-zA-Z_0-9]*):\s*(?P<address>\$[a-fA-F0-9]{2,4})"

        newlines = []

        for line in lines:            
            match = re.search(pattern, line)
            if match is not None:
                d = match.groupdict()
                self.variables[d['varname']] = d['address']
            else:
                newlines.append(line)

        return newlines


    def assemble(self, ifile, ofile):
        lines = self.readlines(ifile)

        try:
            lines = self.read_variables(lines)
            print(self.variables)

            self.lexing(lines)
            self.check_lexing_result()

            if self.show_labels:
                print("\nLABELS\n------------------------------------")
                self.print_labels()

            self.write_vectors(ofile)
            preprocessed_lines = self.preprocessing(lines)

            if self.show_result:
                print("\nRESULT\n-----------------------------------------------------------------------------------------")

            self.assembling(preprocessed_lines, ofile)

        except sly.lex.LexError as err:
            print(f"LEXER ERROR: {err} in phase {self.phase}")
            print(f"[{self.linenumber:5}:    ] {self.current_line.strip()}")
        except opcodes.OpcodeError as err:
            print(f"OPCODE ERROR: {err} in line {self.linenumber}")
        finally:
            self.directives.Finalise()


        