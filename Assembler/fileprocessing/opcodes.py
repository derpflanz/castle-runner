from .lexer import *
import sys

class OpcodeError(SyntaxError):
    _tokens = None

    def __init__(self, message):
        super(OpcodeError, self).__init__(message)

class Opcodes:
    _tokens = None
    _binary = b''
    _length = 0

    def _f_nop(self, mode, address):
        self._binary = b'\xea'

    def _f_lda(self, mode, address):
        if mode == MODE_IMMEDIATE:
            self._binary += b'\xa9'
        elif mode == MODE_ZEROPAGE:
            self._binary += b'\xa5'
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\xad'
        elif mode == MODE_ABSINDEXX:
            self._binary += b'\xbd'
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for LDA")

        self._binary += address


    def _f_ldx(self, mode, address):
        if mode == MODE_IMMEDIATE:
            self._binary += b'\xa2'
        elif mode == MODE_ZEROPAGE:
            self._binary += b'\xa6'
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\xae'
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for LDX")

        self._binary += address


    def _f_ldy(self, mode, address):
        if mode == MODE_IMMEDIATE:
            self._binary += b'\xa0'
        elif mode == MODE_ZEROPAGE:
            self._binary += b'\xa4'
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\xac'
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for LDX")

        self._binary += address

    def _f_sta(self, mode, address):
        if mode == MODE_ZEROPAGE:
            self._binary += b'\x85'
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\x8d'
        elif mode == MODE_ABSINDEXY:
            self._binary += b'\x99'
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for STA")

        self._binary += address


    def _f_inc(self, mode, address):
        if mode == MODE_ZEROPAGE:
            self._binary += b'\xe6'
            self._binary += self._to_bytes(address)
        elif mode == None:      # addressing mode 'A': Accumulator
            self._binary += b'\x1a'
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for INC")

    def _f_jmp(self, mode, address):
        if mode == MODE_ABSOLUTE:
            self._binary += b'\x4c'
            self._binary += address
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for JMP")

    def _f_dec(self, mode, address):
        if mode == None:        # addressing mode 'A': Accumulator
            self._binary += b'\x3a'
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for DEC")

    def _f_jsr(self, mode, address):
        if mode == MODE_ABSOLUTE:
            self._binary += b'\x20'
            self._binary += address
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for JSR")

    def _f_rts(self, mode, address):
        if mode == None:        # addressing mode 's': Stack
            self._binary += b'\x60'
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for DEC")

    def _f_none(self, mode, address):
        return b''

    def _f_brk(self, mode, address):
        self._binary += b'\x00'
    
    def _f_inx(self, mode, address):
        self._binary += b'\xe8'

    def _f_iny(self, mode, address):
        self._binary += b'\xc8'

    def _f_cmp(self, mode, address):
        if mode == MODE_IMMEDIATE:
            self._binary += b'\xc9'
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for CMP")

        self._binary += address

    def _f_beq(self, mode, address):
        self._binary += b'\xf0'
        self._binary += self._relative(address)

    def _f_bne(self, mode, address):
        self._binary += b'\xd0'
        self._binary += self._relative(address)

    _opcode_funcs = {
        None: _f_none,
        'NOP': _f_nop,
        'LDA': _f_lda,
        'STA': _f_sta,
        'INC': _f_inc,
        'JMP': _f_jmp,
        'DEC': _f_dec,
        'JSR': _f_jsr,
        'RTS': _f_rts,
        'BRK': _f_brk,
        'LDX': _f_ldx,
        'INX': _f_inx,
        'LDY': _f_ldy,
        'INY': _f_iny,
        'CMP': _f_cmp,
        'BEQ': _f_beq,
        'BNE': _f_bne
    }

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

            if tok.type == TOK_ASCII:
                value = ord(tok.value.strip("'"))       # remove ' from the token and translate to number
                tok.value = f"{value:02x}"                # translate to two-letter hex code
                tok.type = MODE_IMMEDIATE

        self._process()

    def length(self):
        return len(self._binary)

    def _to_bytes(self, address):
        if type(address) is int:
            address = f"{address:04x}"

        if address.startswith("@") or address.startswith(":"):
            return address

        # address is a string
        # an address can be #$xx, $xx or $xxxx
        address = address.replace('#', '')
        address = address.replace('$', '')
        address = address.replace(',X', '')
        address = address.replace(',Y', '')

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
        length = 0

        for tok in self._tokens:
            if tok.type == TOK_OPCODE:
                opcode = tok.value
                length += 1
            elif tok.type == MODE_IMMEDIATE or tok.type == MODE_ABSOLUTE or tok.type == MODE_ZEROPAGE or \
                    tok.type == MODE_ABSINDEXX or tok.type == MODE_ABSINDEXY or \
                        \
                     (tok.type == TOK_STRINGNAME and opcode is not None) or \
                     (tok.type == TOK_LABEL and opcode is not None) or \
                     (tok.type == TOK_ABSINDEX and opcode is not None):

                addressing_method = tok.type
                address = self._to_bytes(tok.value)

            elif tok.type == TOK_STRING:
                value = tok.value.strip('"')
                length += len(value) + 1
                self._binary += value.encode('ascii')

                # add a null-terminating byte
                self._binary += b'\x00'
        try:
            # we skip the preprocessing addressing modes
            if (addressing_method == TOK_LABEL or addressing_method == TOK_ABSINDEX):
                addressing_method = MODE_ABSOLUTE
                address = b'\x00\x00'

            self._opcode_funcs[opcode](self, addressing_method, address)
        except KeyError:
            raise OpcodeError(f"Opcode {opcode} not supported.")

