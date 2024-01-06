; This is the Castle Runner game!
; It uses the cr001-rom.asm library

; DATA
@SPLASH     "Castle Runner"       ; 13 characters, X=3 to middle it
@COPYRIGHT  "2023"
@GAMEOVER   "Game Over!"

; init
SEI             ; Disable interrupts
CLD             ; Clear "D" flag: use binary mode (instead of BCD)
LDX #$ff        ; Initialise stack on 0x01ff
TXS

; init lcd
JSR :VIO_ResetDisplay
JSR :VIO_InitDisplay
JSR :VIO_ClearDisplay
JSR :InitVideoRam

LDX #$09
LDY #$09
JSR :GotoCharXY

LDA #$00
STA $80
LDA #$C0
STA $81
JSR :WriteString

LDX #$0F            ; (x,y) = (15,100)
LDY #$64
JSR :GotoGraphXY

LDA #$FF            ; test: a black line at (100,15)
JSR :WriteGraph


JSR :VIO_WriteCharScreen
JSR :VIO_WriteGraphScreen

CLI             ; Enable interrupts

:stop
JMP :stop
