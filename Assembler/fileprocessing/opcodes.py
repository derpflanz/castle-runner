from .lexer import *
import sys
import ast

# Opcode Matrix, by addressing mode
# modes: 
# a         Absolute a
# (a,x)     Absolute Indexed indirect 
# a,x       Absolute Indexed with X
# a,y       Absolute Indexed with Y
# (a)       Absolute Indirect
# A         Accumulator
# #         Immediate
# i         Implied
# r         Program Counter Relative
# s         Stack
# zp        Zero Page
# (zp,x)    Zero Page Indexed Indirect
# zp,x      Zero Page Indexed with X
# zp,y      Zero Page Indexed with Y
# (zp)      Zero Page Indirect
# (zp),y    Zero Page Indirect Indexed with Y

_addressing_mode_map = {
    'ABSOLUTE': 0,
    'ABSINDIND': 1,
    'ABSINDEXX': 2,
    'ABSINDEXY': 3,
    'INDIRECT': 4,
    'ACCU': 5,
    'IMMEDIATE': 6,
    'IMPLIED': 7,
    'RELATIVE': 8,
    'STACK': 9,
    'ZEROPAGE': 10,
    'ZPINDIND': 11,
    'ZPINDX': 12,
    'ZPINDY': 13,
    'ZPIND': 14,
    'ZPINDINDY': 15
}

_invalid_addressing_mode = b'\x03'
_opcode_matrix = {}
with open("w65c02s_opcodes.txt", "r") as opcode_file:
    s = opcode_file.read()
    _opcode_matrix = ast.literal_eval(s)
class OpcodeError(SyntaxError):
    _tokens = None

    def __init__(self, message):
        super(OpcodeError, self).__init__(message)

class Opcodes:
    _tokens = None
    _binary = b''
    _length = 0

    def _construct_statement(self, opcode, addressing_mode, operand):
        opcode_record = _opcode_matrix[opcode]
        opcode_mnemonic = None

        if addressing_mode is None:
            # no operand given, this is either A, i or s
            for mode in ['ACCU', 'IMPLIED', 'STACK']:
                addressing_mode_idx = _addressing_mode_map[mode]
                opcode_mnemonic = opcode_record[addressing_mode_idx]
                if opcode_mnemonic != _invalid_addressing_mode:
                    addressing_mode = mode
                    break
        else:
            if addressing_mode == TOK_OPER_ABSOLUTE:
                # operand with two bytes (e.g. $0400) given, this is either a or r
                for mode in ['ABSOLUTE', 'RELATIVE']:
                    addressing_mode_idx = _addressing_mode_map[mode]
                    opcode_mnemonic = opcode_record[addressing_mode_idx]
                    if opcode_mnemonic != _invalid_addressing_mode:
                        addressing_mode = mode
                        break
            else: 
                addressing_mode_idx = _addressing_mode_map[addressing_mode]
                opcode_mnemonic = opcode_record[addressing_mode_idx]

        if opcode_mnemonic == _invalid_addressing_mode:
            raise OpcodeError(f'Addressing mode {addressing_mode} not supported for {opcode}')

        # write mnemonc
        self._binary += opcode_mnemonic

        if addressing_mode is not None:
            if addressing_mode == 'RELATIVE':
                self._binary += self._relative(operand)
            else:
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

    def _to_bytes(self, address):
        if type(address) is int:
            address = f"{address:04x}"

        if address.startswith("@") or address.startswith(":"):
            return address

        if address.startswith("LO"):
            address = address[6:8]
        if address.startswith("HI"):
            address = address[4:6]

        # address is a string
        # an address can be #$xx, $xx or $xxxx
        address = address.replace('#', '')
        address = address.replace('$', '')
        address = address.replace(',X', '')
        address = address.replace(',Y', '')
        address = address.replace('(', '')
        address = address.replace(')', '')

        if len(address) == 1:
            address = '0' + address

        return bytes.fromhex(address)[::-1]

    def as_bytes(self):
        return self._binary

    # the process method takes the tokens and
    # translates them to 6502 assembly binary
    def _process(self):
        opcode = None
        addressing_method = None
        operand = None

        for token in self._tokens:
            if token.type == TOK_OPCODE:
                if opcode is not None:
                    raise SyntaxError("Already saw OPCODE on this line.")

                opcode = token.value
            elif token.type in (TOK_OPER_ABSOLUTE, TOK_OPER_ABSINDIND, TOK_OPER_ABSX, TOK_OPER_ABSY, TOK_OPER_INDIRECT, TOK_OPER_IMMEDIATE,
                                    TOK_OPER_ZP, TOK_OPER_ZPINDIND, TOK_OPER_ZPINDX, TOK_OPER_ZPINDY, TOK_OPER_ZPIND, TOK_OPER_ZPINDINDY):
                if opcode is None:
                    raise SyntaxError("Operand without OPCODE")

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
                value = ord(token.value.strip("'"))       # remove ' from the token and translate to number
                operand = self._to_bytes(f"{value:02x}")                # translate to two-letter hex code
                addressing_method = TOK_OPER_IMMEDIATE
            elif token.type == TOK_COMMENT:
                pass
            else:
                raise SyntaxError(f"Unknown token type: {token.type}. This shouldn't happen.")

        try:
            if opcode is not None:
                self._construct_statement(opcode, addressing_method, operand)
#                self._opcode_funcs[opcode](self, addressing_method, operand)
        except KeyError:
            raise OpcodeError(f"Opcode {opcode} not supported.")