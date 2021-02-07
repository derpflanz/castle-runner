
class Opcodes:
    _tokens = None
    _binary = b''

    def _f_nop(self, mode, address):
        self._binary = b'\xea'

    _opcode_funcs = {
        'NOP': _f_nop
    }

    def __init__(self, tokens):
        self._tokens = tokens
        self._process()

    def as_bytes(self):
        return self._binary

    # the process method takes the tokens and
    # translates them to 6502 assembly binary
    def _process(self):
        opcode = None
        addressing_method = None
        address = None

        for tok in self._tokens:
            if tok.type == 'OPCODE':
                opcode = tok.value
            elif tok.type == 'IMMEDIATEADDR' or tok.type == 'ZEROPAGEADDR' or tok.type == 'ABSOLUTEADDR':
                addressing_method = tok.type
                address = tok.value

        self._opcode_funcs[opcode](self, addressing_method, address)

