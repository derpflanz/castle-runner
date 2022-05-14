; This is the Castle Runner game!
; It uses the cr001-rom.asm library

; DATA
@SPLASH   "Castle Runner"       ; 13 characters, X=3 to middle it
@COPYRIGHT "2021"

@GAMEOVER    "Game Over!"

CLD
LDX #$ff        ; Initialise stack on 0x01ff
TXS

JSR :InitCR
JSR :InitDisplay

; Initialise memory
; Steps left        = $f0          -> when this is 0: Game Over!
; Gold collected    = $f1
; Location row      = $f2
; Location col      = $f3
; Joystick          = $f4
; New location row  = $f5
; New location col  = $f6


; Initialise memory
LDA #$0a            ; we start with 10 steps and no gold
STA $f0
LDA #$00
STA $f1
LDA #$02
STA $f2
LDA #$0a
STA $f3


; Draw fixed content on screen
LDA #$10
JSR :DisplaySetAddress
LDA 'S'
JSR :DisplayChar
LDA #$50
JSR :DisplaySetAddress
LDA 'G'
JSR :DisplayChar


; Main loop
; 1. Draw scores
; 2. Draw x

:loop
JSR :DrawRunner
JSR :PrintGold
JSR :PrintSteps
LDA $f0
BEQ :GameOver

; Read Joystick
LDA $4000
CMP $f4
BEQ :loop               ; joy was not changed

LDA $4000
STA $f4

JSR :ClearRunner
JSR :GetNewLocation
JSR :MoveRunner
JMP :loop

:GameOver
LDA #$01
STA $80
LDA #$05
STA $81
JSR :DisplayGotoXY
LDA LO(@GAMEOVER)
STA $80
LDA HI(@GAMEOVER)
STA $81
JSR :DisplayString

BRK

:MoveRunner
LDA $f5
AND #$fc            ; we allow 4 rows; 0b00 to 0b11
BEQ :__DoMoveRow
RTS
:__DoMoveRow
LDA $f5
STA $f2

LDA $f6
AND #$f0            ; we allow 16 columns: 0b0000 to 0b1111
BEQ :__DoMoveColumn
RTS
:__DoMoveColumn
LDA $f6
STA $f3

DEC $f0
RTS

:GetNewLocation
LDA $f2             ; new location = current location
STA $f5
LDA $f3
STA $f6

LDA $f4
AND #$08
BNE :_cont1
DEC $f6
:_cont1
LDA $f4
AND #$10
BNE :_cont2
INC $f6
:_cont2
LDA $f4
AND #$02
BNE :_cont3
DEC $f5
:_cont3
LDA $f4
AND #$04
BNE :_cont4
INC $f5
:_cont4
RTS

:ClearRunner
LDA $f2
STA $80
LDA $f3
STA $81
JSR :DisplayGotoXY
LDA ' '
JSR :DisplayChar
RTS

:DrawRunner
LDA $f2
STA $80
LDA $f3
STA $81
JSR :DisplayGotoXY
LDA 'X'
JSR :DisplayChar
RTS

:PrintSteps
LDA #$00
STA $80
LDA #$11
STA $81
JSR :DisplayGotoXY
LDA #$40
STA $80
LDA #$00
STA $81
LDA $f0
JSR :Dec2Ascii
JSR :DisplayString
RTS

:PrintGold
LDA #$01
STA $80
LDA #$11
STA $81
JSR :DisplayGotoXY
LDA #$40
STA $80
LDA #$00
STA $81
LDA $f1
JSR :Dec2Ascii
JSR :DisplayString
RTS
