@TEST "Dit is een test"

SEI             ; 78
CLD             ; Clear "D" flag: use binary mode (instead of BCD)
LDX #$ff        ; Initialise stack on 0x01ff
TXS
CLI

LDA #$00
STA $90
LDA #$30
STA $91

LDA LO(@TEST)
STA $80
LDA HI(@TEST)
STA $81

LDY #$ff

:loop
INY
LDA ($80),Y
STA ($90),Y
BNE :loop


:end
BRK

:HW_IRQ
RTI