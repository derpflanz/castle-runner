from .lexer import *

class OpcodeError(SyntaxError):
    _tokens = None

    def __init__(self, message, lineno):
        super(OpcodeError, self).__init__(message)
        self._lineno = lineno

class Opcodes:
    _tokens = None
    _binary = b''

    def _f_nop(self, mode, address):
        self._binary = b'\xea'

    def _f_lda(self, mode, address):
        if mode == MODE_IMMEDIATE:
            self._binary += b'\xa9'
            self._binary += self._to_bytes(address)
        elif mode == MODE_ZEROPAGE:
            self._binary += b'\xa5'
            self._binary += self._to_bytes(address)

    def _f_sta(self, mode, address):
        if mode == MODE_ZEROPAGE:
            self._binary += b'\x85'
            self._binary += self._to_bytes(address)
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\x8d'
            self._binary += self._to_bytes(address)

    def _f_inc(self, mode, address):
        if mode == MODE_ZEROPAGE:
            self._binary += b'\xe6'
            self._binary += self._to_bytes(address)

    def _f_jmp(self, mode, address):
        if mode == MODE_ABSOLUTE:
            self._binary += b'\x4c'
            self._binary += self._to_bytes(address)

    def _f_none(self, mode, address):
        return b''

    _opcode_funcs = {
        None: _f_none,
        'NOP': _f_nop,
        'LDA': _f_lda,
        'STA': _f_sta,
        'INC': _f_inc,
        'JMP': _f_jmp
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

        return bytes.fromhex(address)[::-1]

    def as_bytes(self):
        return self._binary

    # the process method takes the tokens and
    # translates them to 6502 assembly binary
    def process(self):
        opcode = None
        addressing_method = None
        address = None
        lineno = None

        for tok in self._tokens:
            if tok.type == 'OPCODE':
                opcode = tok.value
                lineno = tok.lineno
            elif tok.type == MODE_IMMEDIATE or tok.type == MODE_ABSOLUTE or tok.type == MODE_ZEROPAGE:
                addressing_method = tok.type
                address = tok.value

        try:
            self._opcode_funcs[opcode](self, addressing_method, address)
        except KeyError:
            raise OpcodeError(f"Opcode {opcode} not supported.", lineno)

