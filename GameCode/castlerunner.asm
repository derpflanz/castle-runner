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

; init lcd
JSR :VIO_ResetDisplay
JSR :VIO_InitDisplay
JSR :VIO_ClearDisplay
JSR :InitVideoRam

; For some strange reason, we need this to correctly clear the video ram (wtf?)
LDA #$00
STA $0200

; test code: write data to lower bottom of video ram
LDA #$00        ; GraphX = 0
STA $96

LDA #$00        ; ($12) = $C000
STA $12
LDA #$C0
STA $13

:WriteXs
LDA #$00
STA $10         ; our count-to-40 counter
LDA #$C8        ; GraphY = C8 = 200 = top of bottom frame
STA $97
JSR :CalcGraphPtr

:WriteYs
LDA ($12)
JSR :WriteGraph ; This will INC $97

CLC
LDA $12
ADC #$01
STA $12
LDA $13
ADC #$00
STA $13

INC $10
LDA $10
CMP #$28        ; $28 = 40
BNE :WriteYs
INC $96
LDA $96
CMP #$28
BNE :WriteXs
; end test code


JSR :VIO_WriteCharScreen
JSR :VIO_WriteGraphScreen

CLI             ; Enable interrupts

:stop
JMP :stop
