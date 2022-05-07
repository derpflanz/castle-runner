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
LDA $f4
AND #$08
BNE :_cont1
DEC $f3
DEC $f0
:_cont1
LDA $f4
AND #$10
BNE :_cont2
INC $f3
DEC $f0
:_cont2
LDA $f4
AND #$02
BNE :_cont3
DEC $f2
DEC $f0
:_cont3
LDA $f4
AND #$04
BNE :_cont4
INC $f2
DEC $f0
:_cont4
RTS


:__MoveRunner
LDA $f4
AND #$08
BEQ :_JoyLeft
LDA $f4
AND #$10
BEQ :_JoyRight
LDA $f4
AND #$02
BEQ :_JoyUp
LDA $f4
AND #$04
BEQ :_JoyDown
RTS
:_JoyLeft
DEC $f3
RTS
:_JoyRight
INC $f3
RTS
:_JoyDown
INC $f2
RTS
:_JoyUp
DEC $f2
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
