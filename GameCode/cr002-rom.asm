; This is the CR002 ROM
; 
; This ROM provides routines to use the CR002 computer
; - Video now runs in an interrupt
;
; Memory map:
; VIDEO  0x3000 - 0x3FFF reserved for video related stuff
;        0x3000: Video ptr (i.e. cursor)
;        0x3010: Start of video data
; OUTPUT 0x4000 - 0x4001 are connected to the display
; $80 - $8F     - used for parameters


; NAME      InitCR
; USAGE     JSR :InitCR
; RESULT    Initialises the computer, needs to be called always as first statement
:InitCR
CLI             ; Enable interrupts
CLD             ; Clear "D" flag: use binary mode (instead of BCD)
RTS

; Public display calls

; NAME      DisplayGotoRowCol
; USAGE     LDA <row>
;           STA $80
;           LDA <column>
;           STA $81
;           JSR :DisplayGotoRowCol
; RESULT    The cursor is placed on location <row>,<col>
;           Row and column are 0-based
:DisplayGotoRowCol
LDA #$00
STA $3000           ; video_ptr = 0
LDA $80
CMP #$00            ; if row == 0: goto __add_column
BEQ :__add_column
:__loop_rows
LDA #$13
ADC $3000           ; video_ptr += $13  (20d)
STA $3000
DEC $80             ; row--
BEQ :__add_column   ; if row == 0: goto __add_column
JMP :__loop_rows
:__add_column
LDA $3000           ; video_ptr += col
ADC $81
STA $3000
RTS

; NAME      DisplayGotoLocation
; USAGE     LDA <location>
;           JSR :DisplayGotoLocation
; RESULT    The cursor is placed on location <location>
; NOTE      This call is unnecessary but included for completeness
;           as it just stores the video pointer
:DisplayGotoLocation
STA $3000
RTS


; NAME      DisplayChar
; USAGE     Load character to display in ACCU
;           LDA 'X'
;           JSR :DisplayChar
; RESULT    The character in ACCU put put on display
:DisplayChar
LDX $3000
STA $3010,X
RTS

; NAME      DisplayString
; USAGE     Load lo-byte of start address of string in $80, hibyte in $81
; EXAMPLE   LDA LO(@INFO)
;           STA $80
;           LDA HI(@INFO)
;           STA $81
;           JSR :DisplayString
; RESULT    The string pointed to in $0010-$0011 will put put on display
:DisplayString
LDY #$00            ; y = 0
:_LoopDisplayString
LDA ($80),Y
BEQ :_EndDisplayString
LDX $3000
STA $3010,X
INY
INC $3000
JMP :_LoopDisplayString
:_EndDisplayString
RTS

; Below is actual IO, called from the IRQ handler

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

; Private functions
:_DisplayInstruction
STA $4000           ; DATA = ACCU
LDA #$01            ; RS=0, RW=0, EN=1
STA $4001           ; CTRL = $01
DEC                 ; EN = 0
STA $4001           ; edge it in
RTS