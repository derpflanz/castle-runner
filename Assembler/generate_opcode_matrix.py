import pprint

_opcode_matrix = {
#                 a    (a,x)      a,x      a,y      (a)        A        #        i        r        s       zp   (zp,x)     zp,x     zp,y     (zp)   (zp),y
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
    'WAI': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RMB0': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RMB1': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RMB2': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RMB3': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RMB4': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RMB5': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RMB6': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'RMB7': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SMB0': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SMB1': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SMB2': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SMB3': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SMB4': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SMB5': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SMB6': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'SMB7': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBR0': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBR1': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBR2': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBR3': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBR4': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBR5': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBR6': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBR7': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBS0': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBS1': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBS2': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBS3': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBS4': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBS5': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBS6': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],
    'BBS7': [ b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03', b'\x03' ],

}

_opcode_source = """
BRK:s;  ORA:(zp,x);     ;           ;   TSB:zp;     ORA:zp;     ASL:zp;     RMB0:zp;   PHP:s;  ORA:#;      ASL:A;  ;       TSB:a;      ORA:a;      ASL:a;      BBR0:r
BPL:r;  ORA:(zp),y;     ORA:(zp);   ;   TRB:zp;     ORA:zp,x;   ASL:zp,x;   RMB1:zp;   CLC:i;  ORA:a,y;    INC:A;  ;       TRB:a;      ORA:a,x;    ASL:a,x;    BBR1:r
JSR:a;  AND:(zp,x);     ;           ;   BIT:zp;     AND:zp;     ROL:zp;     RMB2:zp;   PLP:s;  AND:#;      ROL:A;  ;       BIT:a;      AND:a;      ROL:a;      BBR2:r
BMI:r;  AND:(zp),y;     AND:(zp);   ;   BIT:zp,x;   AND:zp,x;   ROL:zp,x;   RMB3:zp;   SEC:i;  AND:a,y;    DEC:A;  ;       BIT:a,x;    AND:a,x;    ROL:a,x;    BBR3:r
RTI:s;  EOR:(zp,x);     ;           ;   ;           EOR:zp;     LSR:zp;     RMB4:zp;   PHA:s;  EOR:#;      LSR:A;  ;       JMP:a;      EOR:a;      LSR:a;      BBR4:r
BVC:r;  EOR:(zp),y;     EOR:(zp);   ;   ;           EOR:zp,x;   LSR:zp,x;   RMB5:zp;   CLI:i;  EOR:a,y;    PHY:s;  ;       ;           EOR:a,x;    LSR:a,x;    BBR5:r
RTS:s;  ADC:(zp,x);     ;           ;   STZ:zp;     ADC:zp;     ROR:zp;     RMB6:zp;   PLA:s;  ADC:#;      ROR:A;  ;       JMP:(a);    ADC:a;      ROR:a;      BBR6:r
BVS:r;  ADC:(zp),y;     ADC:(zp);   ;   STZ:zp,x;   ADC:zp,x;   ROR:zp,x;   RMB7:zp;   SEI:i;  ADC:a,y;    PLY:s;  ;       JMP:(a,x);  ADC:a,x;    ROR:a,x;    BBR7:r
BRA:r;  STA:(zp,x);     ;           ;   STY:zp;     STA:zp;     STX:zp;     SMB0:zp;   DEY:i;  BIT:#;      TXA:i;  ;       STY:a;      STA:a;      STX:a;      BBS0:r
BCC:r;  STA:(zp),y;     STA:(zp);   ;   STY:zp,x;   STA:zp,x;   STX:zp,y;   SMB1:zp;   TYA:i;  STA:a,y;    TXS:i;  ;       STZ:a;      STA:a,x;    STZ:a,x;    BBS1:r
LDY:#;  LDA:(zp,x);     LDX:#;      ;   LDY:zp;     LDA:zp;     LDX:zp;     SMB2:zp;   TAY:i;  LDA:#;      TAX:i;  ;       LDY:A;      LDA:a;      LDX:a;      BBS2:r
BCS:r;  LDA:(zp),y;     LDA:(zp);   ;   LDY:zp,x;   LDA:zp,x;   LDX:zp,y;   SMB3:zp;   CLV:i;  LDA:a,y;    TSX:i;  ;       LDY:a,x;    LDA:a,x;    LDX:a,y;    BBS3:r
CPY:#;  CMP:(zp,x);     ;           ;   CPY:zp;     CMP:zp;     DEC:zp;     SMB4:zp;   INY:i;  CMP:#;      DEX:i;  WAI:i;  CPY:a;      CMP:a;      DEC:a;      BBS4:r
BNE:r;  CMP:(zp),y;     CMP:(zp);   ;   ;           CMP:zp,x;   DEC:zp,x;   SMB5:zp;   CLD:i;  CMP:a,y;    PHX:s;  STP:i;  ;           CMP:a,x;    DEC:a,x;    BBS5:r
CPX:#;  SBC:(zp,x);     ;           ;   CPX:zp;     SBC:zp;     INC:zp;     SMB6:zp;   INX:i;  SBC:#;      NOP:i;  ;       CPX:a;      SBC:a;      INC:a;      BBS6:r
BEQ:r;  SBC:(zp),y;     SBC:(zp);   ;   ;           SBC:zp,x;   INC:zp,x;   SMB7:zp;   SED:i;  SBC:a,y;    PLX:s;  ;       ;           SBC:a,x;    INC:a,x;    BBS7:r
"""

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

lines = _opcode_source.split("\n")
ml_instruction = 0
for line in lines:
    if line.strip() == "":
        continue

    instructions = line.split(";")
    for instruction in instructions:
        parts = instruction.split(":")

        if len(parts) == 2:
            mnenomic = parts[0].strip()
            mode = parts[1].strip()
            
            _opcode_matrix[mnenomic][_addressing_mode_map[mode]] = (ml_instruction).to_bytes(1, byteorder='little')

        ml_instruction += 1

prettyprinter = pprint.PrettyPrinter(depth=2)
prettyprinter.pprint(_opcode_matrix)