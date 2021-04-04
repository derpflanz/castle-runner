from .lexer import *

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
            self._binary += self._to_bytes(address)
        elif mode == MODE_ZEROPAGE:
            self._binary += b'\xa5'
            self._binary += self._to_bytes(address)
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\xad'
            self._binary += self._to_bytes(address)
        elif mode == MODE_ABSINDEXX:
            self._binary += b'\xbd'
            self._binary += self._to_bytes(address)
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for LDA")

    def _f_ldx(self, mode, address):
        if mode == MODE_IMMEDIATE:
            self._binary += b'\xa2'
            self._binary += self._to_bytes(address)
        elif mode == MODE_ZEROPAGE:
            self._binary += b'\xa6'
            self._binary += self._to_bytes(address)
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\xae'
            self._binary += self._to_bytes(address)
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for LDX")

    def _f_ldy(self, mode, address):
        if mode == MODE_IMMEDIATE:
            self._binary += b'\xa0'
            self._binary += self._to_bytes(address)
        elif mode == MODE_ZEROPAGE:
            self._binary += b'\xa4'
            self._binary += self._to_bytes(address)
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\xac'
            self._binary += self._to_bytes(address)
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for LDX")


    def _f_sta(self, mode, address):
        if mode == MODE_ZEROPAGE:
            self._binary += b'\x85'
            self._binary += self._to_bytes(address)
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\x8d'
            self._binary += self._to_bytes(address)
        else:
            raise OpcodeError(f"Addressing mode {mode} not supported for STA")

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
            self._binary += self._to_bytes(address)
        elif mode == TOK_LABEL:
            # a label implies absolute addressing
            self._binary += b'\x4c'
            self._binary += self._label_to_address(address)
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
            self._binary += self._to_bytes(address)
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

    def _label_to_address(self, label):
        if self._lookup_labels is True:
            try:
                return self._to_bytes(self._labels[label])
            except KeyError:
                raise SyntaxError(f"Label {label} not found.")
        else:
            return b'\x00\x00'        

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
        'LDY': _f_ldy
    }

    def __init__(self, tokens, labels = None, lookup_labels = True):
        self._tokens = tokens or []
        self._labels = labels or {}
        self._lookup_labels = lookup_labels

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
        return self._length

    def _to_bytes(self, address):
        # address is a string
        # an address can be #$xx, $xx or $xxxx
        address = address.replace('#', '')
        address = address.replace('$', '')
        address = address.replace(',X', '')

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
                     (tok.type == TOK_ABSINDEXY_S and opcode is not None) or \
                     (tok.type == TOK_ABSINDEXX_S and opcode is not None):
                     
                addressing_method = tok.type
                address = tok.value

                if tok.type == MODE_ABSOLUTE or tok.type == MODE_ABSINDEXX or tok.type == TOK_LABEL or tok.type == MODE_ABSINDEXY:
                    length += 2
                else:
                    length += 1

            elif tok.type == TOK_STRING:
                value = tok.value.strip('"')
                length += len(value)
                self._binary += value.encode('ascii')

        self._length = length

        try:
            # we skip the preprocessing addressing modes
            if not (addressing_method == TOK_LABEL or addressing_method == TOK_ABSINDEXX_S or addressing_method == TOK_ABSINDEXY_S):
                self._opcode_funcs[opcode](self, addressing_method, address)
        except KeyError:
            raise OpcodeError(f"Opcode {opcode} not supported.")

