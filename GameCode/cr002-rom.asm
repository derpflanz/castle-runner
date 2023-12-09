; This is the CR002 ROM
; 
; This ROM provides routines to use the CR002 computer
; - Video now runs in an interrupt
;
; Memory map:
; VIDEO  0x3000 - 0x3FFF reserved for video related stuff
;        0x3000: Video ptr (i.e. cursor)
;        0x3001: LCD init (00 = not initalised, 01 = initialised)
;        0x3010: Start of video data
; OUTPUT 0x4000 - 0x4001 are connected to the display
; $80 - $8F     - used for parameters


; NAME      InitCR
; USAGE     JSR :InitCR
; RESULT    Initialises the computer, needs to be called always as first statement
:InitCR
CLI             ; Enable interrupts
CLD             ; Clear "D" flag: use binary mode (instead of BCD)
LDA #$00        ; one shot to init LCD
STA $3001
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

; NAME      Dec2Ascii
; USAGE     Load byte to convert into ACCU, and target address in $80-$81
; EXAMPLE   LDA #$40
;           STA $80
;           LDA #$00
;           STA $81
;           LDA #$bf
;           JSR :Dec2Ascii
; RESULT    The string pointed to in $80 will have the decimal in ascii, as a zero terminated string
:Dec2Ascii
PHA
LDY #$00        ; initialise target (000\0)
LDA '0'
STA ($80),Y
INY
STA ($80),Y
INY
STA ($80),Y
INY
LDA #$00
STA ($80),Y
LDY #$00
PLA

:__hundreds     ; subtract hundreds until we go through zero
SEC
SBC #$64        ; $64 = 100
BCC :__tens_start
PHA
CLC
LDA ($80),Y
ADC #$01
STA ($80),Y
PLA
JMP :__hundreds

:__tens_start   ; subtract tens until we go through zero
ADC #$64
INY
:__tens
SEC
SBC #$0A
BCC :__ones_start
PHA
CLC
LDA ($80),Y
ADC #$01
STA ($80),Y
PLA
JMP :__tens

:__ones_start   ; subtract ones until we go through zero
ADC #$0A
INY
:__ones
SEC
SBC #$01
BCC :__done
PHA
CLC
LDA ($80),Y
ADC #$01
STA ($80),Y
PLA

JMP :__ones

:__done
RTS

; Below is actual IO, called from the IRQ handler
:HW_IRQ
SEI
LDA $3001
BNE :_UpdateDisplay

LDA #$01
STA $3001

; #### Display INIT
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

:_UpdateDisplay

; 0-row1 40-row2 14-row3 54-row4


CLI             ; Enable intterupts
RTI             ; And we're done


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
