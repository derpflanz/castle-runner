SEI         ; 78
CLD             ; Clear "D" flag: use binary mode (instead of BCD)
LDX #$ff        ; Initialise stack on 0x01ff
TXS
CLI

LDA #$00
STA $4000

LDA #$00
STA $4001

:loop
LDA #$01
STA $4000

LDA #$ff        ; number of full cycles to make $ff = 8ms
STA $80
JSR :__delay1

LDA #$00
STA $4000

LDA #$ff        ; number of full cycles to make $ff = 8ms
STA $80
JSR :__delay1

JMP :loop

:__delay1
LDA #$ff
:_inner1
DEC
BNE :_inner1
DEC $80
BNE :__delay1
RTS

:HW_IRQ
RTI