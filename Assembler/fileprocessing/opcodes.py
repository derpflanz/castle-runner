from .lexer import *
import sys

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
    'a': 0,
    '(a,x)': 1,
    'a,x': 2,
    'a,y': 3,
    '(a)': 4,
    'A': 5,
    '#': 6,
    'i': 7,
    'r': 8,
    's': 9,
    'zp': 10,
    '(zp,x)': 11,
    'zp,x': 12,
    'zp,y': 13,
    '(zp)': 14,
    '(zp),y': 15
}

_unimplemented = b'\x03'
_invalid_addressing_mode = b'\x02'

_opcode_matrix = {
#                 a        1        2        3        4        5        #        7        r        s       10       11       12       13       14       15
    'ADC': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'AND': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'ASL': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBR': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBS': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BCC': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x90', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BCS': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\xb0', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BEQ': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\xf0', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BIT': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BMI': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x30', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BNE': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\xd0', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BPL': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x10', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BRA': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x80', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BRK': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x00', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BVC': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x50', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BVS': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x70', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'CLC': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'CLD': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'CLI': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'CLV': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'CMP': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'CPX': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\xe0', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'CPY': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\xc0', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'DEC': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'DEX': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'DEY': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'EOR': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'INC': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'INX': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'INY': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'JMP': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'JSR': [ b'\x20', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'LDA': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'LDX': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'LDY': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\xa0', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'LSR': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'NOP': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'ORA': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'PHA': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'PHP': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'PHX': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'PHY': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'PLA': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'PLP': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'PLX': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'PLY': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RMB': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'ROL': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'ROR': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RTI': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x40', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RTS': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x60', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SBC': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SEC': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SED': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SEI': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SMB': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'STA': [ b'\x8d', b'\x81', b'\x9d', b'\x99', b'\x02', b'\x02', b'\x02', b'\x02', b'\x02', b'\x02', b'\x85', b'\x02', b'\x95', b'\x02', b'\x02', b'\x02' ],
    'STP': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'STX': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'STY': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'STZ': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'TAX': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'TAY': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'TRB': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'TSB': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'TSX': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'TXA': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'TXS': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'TYA': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'WAI': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ]
}

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
            for mode in ['A', 'i', 's']:
                addressing_mode_idx = _addressing_mode_map[mode]
                opcode_mnemonic = opcode_record[addressing_mode_idx]
                if opcode_mnemonic != _invalid_addressing_mode:
                    addressing_mode = mode
                    break
        else:
            if addressing_mode == MODE_ABSOLUTE:
                # operand with two bytes (e.g. $0400) given, this is either a or r
                for mode in ['a', 'r']:
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

        if opcode_mnemonic == _unimplemented:
            raise OpcodeError(f'Addressing mode {addressing_mode} not implemented for {opcode}')

        # write mnemonc
        self._binary += opcode_mnemonic

        if addressing_mode is not None:
            if addressing_mode == MODE_RELATIVE:
                self._binary += self._relative(operand)
            else:
                self._binary += operand


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
                self._construct_statement(opcode, addressing_method, operand)
#                self._opcode_funcs[opcode](self, addressing_method, operand)
        except KeyError:
            raise OpcodeError(f"Opcode {opcode} not supported.")