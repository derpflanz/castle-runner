from .lexer import *

class OpcodeError(SyntaxError):
    _tokens = None

    def __init__(self, message, lineno):
        super(OpcodeError, self).__init__(message)
        self._lineno = lineno

class Opcodes:
    _tokens = None
    _binary = b''
    _length = 0

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
        self._tokens = tokens or []

        # a valid Opcodes object has an opcode, followed by an addressing mode
        # comments are ignored
        saw_opcode = False
        saw_address = False
        for tok in self._tokens:
            if tok.type == TOK_OPCODE:
                if saw_address:
                    raise SyntaxError("Opcode cannot be after address.")
                if saw_opcode:
                    raise SyntaxError("Cannot have two opcodes in a line")
                saw_opcode = True

            if tok.type == MODE_ABSOLUTE or tok.type == MODE_IMMEDIATE or tok.type == MODE_ZEROPAGE:
                if saw_address:
                    raise SyntaxError("Cannot have two addresses in a line")
                saw_address = True

        self._process()

    def length(self):
        return self._length

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
    def _process(self):
        opcode = None
        addressing_method = None
        address = None
        lineno = None
        length = 0

        for tok in self._tokens:
            if tok.type == 'OPCODE':
                opcode = tok.value
                lineno = tok.lineno
                length += 1                
            elif tok.type == MODE_IMMEDIATE or tok.type == MODE_ABSOLUTE or tok.type == MODE_ZEROPAGE:
                addressing_method = tok.type
                address = tok.value

                if tok.type == MODE_ABSOLUTE:
                    length += 2
                else:
                    length += 1

        self._length = length

        try:
            self._opcode_funcs[opcode](self, addressing_method, address)
        except KeyError:
            raise OpcodeError(f"Opcode {opcode} not supported.", lineno)

