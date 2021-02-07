from .lexer import *

class OpcodeError(SyntaxError):
    pass

class Opcodes:
    _tokens = None
    _binary = b''

    def _f_nop(self, mode, address):
        self._binary = b'\xea'

    def _f_lda(self, mode, address):
        if mode == MODE_IMMEDIATE:
            self._binary += b'\xa9'
            self._binary += self._to_bytes(address)

    _opcode_funcs = {
        'NOP': _f_nop,
        'LDA': _f_lda
    }

    def __init__(self, tokens):
        self._tokens = tokens

    def _to_bytes(self, address):
        # address is a string
        # an address can be #$xx, $xx or $xxxx
        address = address.replace('#', '')
        address = address.replace('$', '')

        if len(address) == 1:
            address = '0' + address

        return bytes.fromhex(address)

    def as_bytes(self):
        return self._binary

    # the process method takes the tokens and
    # translates them to 6502 assembly binary
    def process(self):
        opcode = None
        addressing_method = None
        address = None

        for tok in self._tokens:
            if tok.type == 'OPCODE':
                opcode = tok.value
            elif tok.type == MODE_IMMEDIATE or tok.type == MODE_ABSOLUTE or tok.type == MODE_ZEROPAGE:
                addressing_method = tok.type
                address = tok.value

        try:
            self._opcode_funcs[opcode](self, addressing_method, address)
        except KeyError:
            raise OpcodeError(f"Opcode {opcode} not supported.")

