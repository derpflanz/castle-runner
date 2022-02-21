; This is the Castle Runner game!

; This ASM shows U/D/L/R/F when using the joystick
; It uses the cr001-rom.asm library

; to assemble: ./assemble.sh asm/joystick-demo.asm asm/cr001-rom.asm

; DATA
@SPLASH   "Castle Runner"       ; 13 characters, X=3 to middle it

CLD
LDX #$ff        ; Initialise stack on 0x01ff
TXS

; We need the display, so init that first
JSR :InitCR
JSR :InitDisplay

LDA #$03
STA $80
LDA #$01
STA $81
JSR :DisplayGotoXY

LDA LO(@SPLASH)
STA $80
LDA HI(@SPLASH)
STA $81
JSR :DisplayString

BRK