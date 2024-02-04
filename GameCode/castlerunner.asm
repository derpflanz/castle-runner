; This is the Castle Runner game!
; It uses the cr001-rom.asm library

; DATA
@SPLASH     "123456789ABCDEF0123456789"       ; 13 characters, X=3 to middle it
@COPYRIGHT  "2023"
@GAMEOVER   "Game Over!"

; init
SEI             ; Disable interrupts
CLD             ; Clear "D" flag: use binary mode (instead of BCD)
LDX #$ff        ; Initialise stack on 0x01ff
TXS

LDA #$10        ; x, y
STA $96
LDA #$78
STA $97
JSR :CalcGraphPtr
BRK

LDX #$00
LDY #$01
JSR :GotoGraphXY
;JSR :GotoCharXY
BRK

; init lcd
JSR :VIO_ResetDisplay
JSR :VIO_InitDisplay
JSR :VIO_ClearDisplay
JSR :InitVideoRam


LDX #$01
LDY #$01
JSR :GotoCharXY

LDA LO(@SPLASH)
STA $80
LDA HI(@SPLASH)
STA $81
JSR :WriteString

LDA #$00            ; ($10) = $C000 (data section)
STA $10
LDA #$C0
STA $11

LDX #$00            ; (x,y) = (0,0)
LDY #$00
JSR :GotoGraphXY

LDY #$00
:firstloop
LDA ($10),y
JSR :WriteGraph
INY
CPY #$0f
BNE :firstloop

; LDX #$01            ; (x,y) = (1,0)
; LDY #$00
; JSR :GotoGraphXY
; LDY #$10
; :secondloop
; LDA ($10),y
; JSR :WriteGraph
; INY
; CPY #$1f
; BNE :secondloop

LDX #$10
LDY #$78
JSR :GotoGraphXY

LDA #$30
JSR :WriteGraph
LDA #$48
JSR :WriteGraph
LDA #$84
JSR :WriteGraph
LDA #$84
JSR :WriteGraph
LDA #$48
JSR :WriteGraph
LDA #$30
JSR :WriteGraph



JSR :VIO_WriteCharScreen
JSR :VIO_WriteGraphScreen

CLI             ; Enable interrupts

:stop
JMP :stop
