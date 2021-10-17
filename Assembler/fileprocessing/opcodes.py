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
        elif mode == MODE_ZPINDINDY:
            self._binary += b'\xb1'
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
            self._binary += address
        elif mode == MODE_ABSOLUTE:
            self._binary += b'\xee'
            self._binary += address
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

    def _f_txs(self, mode, address):
        self._binary += b'\x9a'

    def _f_sei(self, mode, address):
        self._binary += b'\x78'

    def _f_cld(self, mode, address):
        self._binary += b'\xd8'

    _opcode_funcs = {
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
        'BNE': _f_bne,
        'TXS': _f_txs,
        'SEI': _f_sei,
        'CLD': _f_cld
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

        for tok in self._tokens:
            if tok.type == TOK_OPCODE:
                if opcode is not None:
                    raise SyntaxError("Already saw OPCODE on this line.")

                opcode = tok.value
            elif tok.type in (MODE_IMMEDIATE, MODE_ABSOLUTE, MODE_ABSINDEXX, MODE_ABSINDEXY, MODE_ZEROPAGE, MODE_ZPINDINDY):
                if opcode is None:
                    raise SyntaxError("Operand without OPCODE")

                # 'real' addressing modes
                addressing_method = tok.type
                operand = self._to_bytes(tok.value)
            elif tok.type == TOK_STRING:
                # verbatim string
                value = tok.value.strip('"')
                self._binary += value.encode('ascii')
                self._binary += b'\x00'
            elif tok.type in (TOK_STRINGNAME, TOK_LABEL, TOK_ABSINDEX):
                # addresses that will be translated in a later run to a real address
                # we only need to put them in for the opcode length
                addressing_method = MODE_ABSOLUTE
                operand = b'\x00\x00'
            elif tok.type in (TOK_HI, TOK_LO):
                addressing_method = MODE_IMMEDIATE
                operand = b'\x00'
            elif tok.type == TOK_ASCII:
                value = ord(tok.value.strip("'"))       # remove ' from the token and translate to number
                operand = self._to_bytes(f"{value:02x}")                # translate to two-letter hex code
                addressing_method = MODE_IMMEDIATE
            elif tok.type == TOK_COMMENT:
                pass
            else:
                raise SyntaxError(f"Unknown token type: {tok.type}. This shouldn't happen.")

        try:
            if opcode is not None:
                self._opcode_funcs[opcode](self, addressing_method, operand)
        except KeyError:
            raise OpcodeError(f"Opcode {opcode} not supported.")