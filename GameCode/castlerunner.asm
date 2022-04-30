; This is the Castle Runner game!
; It uses the cr001-rom.asm library

; DATA
@SPLASH   "Castle Runner"       ; 13 characters, X=3 to middle it
@COPYRIGHT "2021"

CLD
LDX #$ff        ; Initialise stack on 0x01ff
TXS

; We need the display, so init that first
JSR :InitCR
JSR :InitDisplay

LDA #$00
STA $00         ; row

LDA #$00
STA $01         ; column

:resetabc
LDA 'A'
STA $02

:loop
LDA $00
STA $80
LDA $01
STA $81
JSR :DisplayGotoXY

LDA $02
JSR :DisplayChar

INC $01
LDA $01
CMP #$14        ; hex14=dec20 = end of line
BNE :continuecol
LDA #$00
STA $01         ; reset col 
INC $00         ; inc row
LDA $00
CMP #$04
BNE :continuecol
LDA #$00        ; reset row
STA $00

:continuecol

; check abc
INC $02
LDA $02
CMP '['
BEQ :resetabc

JMP :loop


BRK