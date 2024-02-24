from .lexer import *
import sys, ast

_addressing_mode_map = {
    'ABSOLUTE': 0,          # a         Absolute a
    'ABSINDIND': 1,         # (a,x)     Absolute Indexed indirect 
    'ABSINDEXX': 2,         # a,x       Absolute Indexed with X
    'ABSINDEXY': 3,         # a,y       Absolute Indexed with Y
    'INDIRECT': 4,          # (a)       Absolute Indirect
    'ACCU': 5,              # A         Accumulator
    'IMMEDIATE': 6,         # #         Immediate
    'IMPLIED': 7,           # i         Implied
    'RELATIVE': 8,          # r         Program Counter Relative
    'STACK': 9,             # s         Stack
    'ZEROPAGE': 10,         # zp        Zero Page
    'ZPINDIND': 11,         # (zp,x)    Zero Page Indexed Indirect
    'ZPINDX': 12,           # zp,x      Zero Page Indexed with X
    'ZPINDY': 13,           # zp,y      Zero Page Indexed with Y
    'ZPIND': 14,            # (zp)      Zero Page Indirect
    'ZPINDINDY': 15         # (zp),y    Zero Page Indirect Indexed with Y
}

_invalid_addressing_mode = b'\x03'
_opcode_matrix = {}

def opcode_init(opcode_filename):
    global _opcode_matrix

    with open(opcode_filename, "r") as opcode_file:
        s = opcode_file.read()
        _opcode_matrix = ast.literal_eval(s)

class OpcodeError(SyntaxError):
    pass

class Opcodes:
    _tokens = None
    _binary = b''
    _length = 0
    _variables = {}

    def _construct_statement(self, opcode, addressing_mode, operand):
        opcode_record = _opcode_matrix[opcode]
        opcode_binary = None

        if addressing_mode is None:
            # no operand given, this is either Accu, Implied or Stack
            for mode in ['ACCU', 'IMPLIED', 'STACK']:
                addressing_mode_idx = _addressing_mode_map[mode]
                opcode_binary = opcode_record[addressing_mode_idx]
                if opcode_binary != _invalid_addressing_mode:
                    addressing_mode = mode
                    break
        else:
            if addressing_mode == TOK_OPER_ABSOLUTE:
                # operand with two bytes (e.g. $0400) given, this is either Absolute or Relative
                for mode in ['ABSOLUTE', 'RELATIVE']:
                    addressing_mode_idx = _addressing_mode_map[mode]
                    opcode_binary = opcode_record[addressing_mode_idx]
                    if opcode_binary != _invalid_addressing_mode:
                        addressing_mode = mode
                        break
            else: 
                addressing_mode_idx = _addressing_mode_map[addressing_mode]
                opcode_binary = opcode_record[addressing_mode_idx]

        if opcode_binary == _invalid_addressing_mode:
            raise OpcodeError(f'Addressing mode {addressing_mode} not supported for {opcode}')

        # write mnemonc
        self._binary += opcode_binary

        if addressing_mode is not None:
            if addressing_mode == 'RELATIVE':
                self._binary += self._relative(operand)
            elif addressing_mode != 'IMPLIED' and addressing_mode != 'STACK' and addressing_mode != 'ACCU':
                self._binary += operand

    def _relative(self, address):
        start_address = int.from_bytes(self._address, 'little')
        to_address = int.from_bytes(address, 'little')

        diff = to_address - start_address
        diff -= 2
        
        if diff < 0:
            diff += 256

        return int.to_bytes(diff,1, 'little')

    def __init__(self, tokens, address = 0):
        self._tokens = tokens or []
        self._address = self._to_bytes(address)
        self._process()

    def length(self):
        return len(self._binary)

    def _to_bytes(self, operand):
        if type(operand) is int:
            operand = f"{operand:04x}"

        if operand.startswith("@") or operand.startswith(":"):
            return operand

        if operand.startswith("LO"):
            operand = operand[6:8]
        if operand.startswith("HI"):
            operand = operand[4:6]

        # operand is a string
        # an operand can be #$xx, $xx or $xxxx
        operand = operand.replace('#', '')
        operand = operand.replace('$', '')
        operand = operand.replace(',X', '')
        operand = operand.replace(',Y', '')
        operand = operand.replace(',x', '')
        operand = operand.replace(',y', '')
        operand = operand.replace('(', '')
        operand = operand.replace(')', '')

        if len(operand) == 1:
            operand = '0' + operand

        return bytes.fromhex(operand)[::-1]

    def as_bytes(self):
        return self._binary

    # the process method takes the tokens and
    # translates them to 6502 assembly binary
    def _process(self):
        opcode = None
        addressing_method = None
        operand = None
        variable_name = None

        for token in self._tokens:
            if token.type == TOK_OPCODE:
                if opcode is not None:
                    raise SyntaxError("Already saw OPCODE on this line.")

                opcode = token.value
            elif token.type in (TOK_OPER_ABSOLUTE, TOK_OPER_ABSINDIND, TOK_OPER_ABSX, TOK_OPER_ABSY, TOK_OPER_INDIRECT, TOK_OPER_IMMEDIATE,
                                    TOK_OPER_ZP, TOK_OPER_ZPINDIND, TOK_OPER_ZPINDX, TOK_OPER_ZPINDY, TOK_OPER_ZPIND, TOK_OPER_ZPINDINDY):

                if opcode is None and variable_name is None:
                    raise SyntaxError("Operand without OPCODE or VARIABLE_NAME")

                if variable_name is not None:                                   # register variable value
                    Opcodes._variables[variable_name] = token.value

                if f"{token.value}:" in Opcodes._variables:                     # fetch value of variable
                    token.value = Opcodes._variables[f"{token.value}:"]

                # 'real' addressing modes
                addressing_method = token.type
                operand = self._to_bytes(token.value)
            elif token.type == TOK_STRING:
                # verbatim string
                value = token.value.strip('"')
                self._binary += value.encode('ascii')
                self._binary += b'\x00'
            elif token.type in (TOK_STRINGNAME, TOK_LABEL, TOK_STRINGDEREF):
                # addresses that will be translated in a later run to a real address
                # we only need to put them in for the opcode length
                addressing_method = TOK_OPER_ABSOLUTE
                operand = b'\x00\x00'
            elif token.type in (TOK_HI, TOK_LO):
                addressing_method = TOK_OPER_IMMEDIATE
                operand = b'\x00'
            elif token.type == TOK_ASCII:
                value = ord(token.value.strip("'"))                     # remove ' from the token and translate to number
                operand = self._to_bytes(f"{value:02x}")                # translate to two-letter hex code
                addressing_method = TOK_OPER_IMMEDIATE
            elif token.type in (TOK_COMMENT, TOK_DIRECTIVE):
                pass
            elif token.type in (TOK_VARIABLE):
                variable_name = token.value
            else:
                raise SyntaxError(f"Unknown token type: {token.type}. This shouldn't happen.")

        try:
            if opcode is not None:
                self._construct_statement(opcode, addressing_method, operand)
        except KeyError:
            raise OpcodeError(f"Opcode {opcode} not supported.")