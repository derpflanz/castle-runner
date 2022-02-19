; This is the CR001 ROM
; 
; This ROM provides routines to use the CR001 computer

; NAME      InitCR
; USAGE     JSR :InitCR
; RESULT    Initialises the computer, needs to be called always as first statement
:InitCR
SEI             ; Disable interrupts
CLD             ; Clear "D" flag: use binary mode (instead of BCD)
RTS

; NAME      InitDisplay
; USAGE     JSR :InitDisplay
; RESULT    Initialises the display. It will be set to 8-bit mode, cleared and switched on.
:InitDisplay
LDA #$00
STA $4000       ; DATA = 0
STA $4001       ; CTRL = 0

LDA #$38        ; set 8 bit mode; 2 line display; 5x8 dot char
JSR :_DisplayInstruction
LDA #$01        ; clear display
JSR :_DisplayInstruction
LDA #$0C        ; display on, cursor off, blinking off
JSR :_DisplayInstruction
LDA #$06        ; increase on write, no display shift
JSR :_DisplayInstruction
RTS             ; /InitDisplay

:_DisplayEdge
DEC                 ; EN = 0
STA $4001           ; edge it in
RTS

:_DisplayInstruction
STA $4000           ; DATA = ACCU
LDA #$01            ; RS=0, RW=0, EN=1
STA $4001           ; CTRL = $01
JSR :_DisplayEdge
RTS

; NAME      DisplayHome
; USAGE     JSR :DisplayHome           
; RESULT    The cursor is put to the upper left corner
:DisplayHome
LDA #$02        ; Return home
JSR :_DisplayInstruction
RTS             ; /DisplayHome

; NAME      DisplaySetAddress
; USAGE     Load address in ACCU (max 80 bytes $00 - $50)
;           JSR :DisplaySetAddress
; RESULT    The cursor is set to the address, subsequent 
;           writes start there
:DisplaySetAddress
ORA #$80        ; MSB should be set (see p191 in datasheet)
JSR :_DisplayInstruction
RTS

; NAME      DisplayGotoXY
; USAGE     LDA <position>
;           PHA
;           LDA <line>
;           PHA
;           JSR :DisplayGotoXY
; RESULT    The cursor is placed on location X,Y
;           X = position, Y = line, 0-based
:DisplayGotoXY
; In the display, lines are 0-2-1-3
LDA $81
CMP #$01
BEQ :__line_1__
CMP #$02
BEQ :__line_2__
CMP #$03
BEQ :__line_3__
LDA #$00            ; fallback: line 0
JMP :__gotoxy_continued__

:__line_1__
LDA #$40            ; hex 28 = dec 40 = line 1
JMP :__gotoxy_continued__
:__line_2__
LDA #$14            ; hex 14 = dec 20 = line 2
JMP :__gotoxy_continued__
:__line_3__
LDA #$54            ; hex 3c = dec 60 = line 3

:__gotoxy_continued__
CLC
ADC $80         ; ACCU += x
JSR :DisplaySetAddress
RTS             ; /DisplayGotoXY

; NAME      DisplayChar
; USAGE     Load character to display in ACCU 
;           JSR :DisplayChar
; RESULT    The character in ACCU put put on display
:DisplayChar
STA $4000           ; data = ACCU
LDA #$05            ; RS=1, RW=0, EN=1
STA $4001           ; ctrl = ACCU
JSR :_DisplayEdge
RTS         ; /DisplayChar

; NAME      DisplayString
; USAGE     Load lo-byte of start address of string in $10, hibyte in $11
;           JSR :DisplayString
; RESULT    The string pointed to in $0010-$0011 will put put on display
:DisplayString
LDY #$00            ; y = 0
:_LoopDisplayString
LDA ($80),Y
BEQ :_EndDisplayString
JSR :DisplayChar
INY
JMP :_LoopDisplayString

:_EndDisplayString
RTS         ; /DisplayString
