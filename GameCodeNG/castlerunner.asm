; This is the Castle Runner game!
; It uses the cr001-rom.asm library

; DATA
splash = "123456789ABCDEF0123456789"       ; 13 characters, X=3 to middle it
copyright = "2023"
gameover = "Game Over!"

; init
SEI             ; Disable interrupts
CLD             ; Clear "D" flag: use binary mode (instead of BCD)
LDX #$ff        ; Initialise stack on 0x01ff
TXS

; init lcd
JSR VIO_ResetDisplay
JSR VIO_InitDisplay
JSR VIO_ClearDisplay
JSR InitVideoRam

; For some strange reason, we need this to correctly clear the video ram (wtf?)
LDA #$00
STA $0200

JSR LoadBackdrop
JSR VIO_WriteGraphScreen

CLI             ; Enable interrupts

; ======================= INITIALISATION ==================================
; initialise location of our runner
LDA #$0F
STA $C0
LDA #$14
STA $C1

LDA #$10
STA $C2

LDA $4000       ; Joystick shadow
STA $D0

; ======================= GAME LOOP ==================================
GameLoop:
; Read joystick value and store in temp
LDA $4000
STA $D1

; Read joystick value and output to screen (for debug)
LDA #$29
STA $80
LDA #$02
STA $81
LDA $D1
JSR Dec2Ascii

; Print steps
LDA #$4E
STA $80
LDA #$06
STA $81
LDA $C2
JSR Dec2Ascii

; Read joystick and change XY for character
LDA $D1           ; When joystick hasn't changed, do nothing
CMP $D0
BEQ GameLoop

LDA $D1
STA $D0

LDA $D1             ; When joy in (or back to) middle, don't do anything
CMP #$FF
BEQ GameLoop

LDA $C0             ; Remove old Runner
STA $94
LDA $C1
STA $95
JSR CalcCharPtr
LDA ' '
JSR WriteChar

LDA $D1
AND #$02            ; UP
BNE Joy1
DEC $C0
Joy1:
LDA $D1
AND #$04            ; DOWN
BNE Joy2
INC $C0
Joy2:
LDA $D1
AND #$08            ; LEFT
BNE Joy3
DEC $C1
Joy3:
LDA $D1
AND #$10            ; RIGHT
BNE Joy4
INC $C1
Joy4:

LDA $C0             ; Write new Runner
STA $94
LDA $C1
STA $95
JSR CalcCharPtr
LDA 'X'
JSR WriteChar

DEC $C2

JSR VIO_WriteCharScreen
JMP GameLoop

LoadBackdrop:
    ; Data starting in $C000 is our game backdrop
    ; Load it into the Video RAM
    LDA #$00        ; GraphX = 0
    STA $96

    LDA #$00        ; ($12) = $C000
    STA $12
    LDA #$C0
    STA $13

    WriteXs:
    LDA #$00
    STA $10         ; our count-to-40 counter
    LDA #$C8        ; GraphY = C8 = 200 = top of bottom frame
    STA $97
    JSR CalcGraphPtr

    WriteYs:
    LDA ($12)
    JSR WriteGraph ; This will INC $97

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
    BNE WriteYs
    INC $96
    LDA $96
    CMP #$28
    BNE WriteXs
RTS
