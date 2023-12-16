; This is the CR002 ROM
; 
; This ROM provides routines to use the CR002 computer
; - Video now runs in an interrupt and is 320x240 pixels big
;
; Memory map:
; VIDEO  0x0200 - 0x2FFF reserved for video related stuff (12kB)
;        0x0200: Video ptr (i.e. cursor)
;        0x0201: LCD init (00 = not initalised, 01 = initialised)
;        0x0202: Control register to be sent
;        0x0210: Start of video data
; OUTPUT 0x4000 is display data (D0-D7)
;        0x4001 is display control:
;                   bit0: /WR
;                   bit1: RS (1=cmd, 0=data)
;                   bit2: /RESET
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

:InitDisplay
; #### Display INIT
LDA #$00
STA $4000       ; DATA = 0
STA $4001       ; CTRL = 0

; 1. RESET ROUTINE
LDA #$01        ; WR=1, RS=0, RES=0
STA $0202
STA $4001

LDA #$ff        ; number of full cycles to make $ff = 8ms
STA $80

:__delay
LDA #$ff
:_aa
DEC
BNE :_aa
LDA $80
DEC
STA $80
BNE :__delay

LDA #$05        ; WR=1, RS=0, RES=1
STA $4001

; 2. Display initialisation
LDA #$40            ; Initializes device and display
JSR :__comm_out

LDA #$30            ; Memory Configuration Register        0x8000, 0x03   --> Config: Origin Comp = 1, Single Panel, Char Height = 8 pixels, CGROM selected
JSR :__data_out
LDA #$87            ; Horizontal Character Size            0x8001, 0x07   --> HCS  =   8
JSR :__data_out
LDA #$07            ; Vertical Character Size              0x8002, 0x07   --> VCS  =   8
JSR :__data_out
LDA #$27            ; Character Bytes Per Row              0x8003, 0x27   --> C/R  =  40 chars
JSR :__data_out
LDA #$50            ; Total Character Bytes Per Row        0x8004, 0x50   --> TC/R =  79
JSR :__data_out
LDA #$EF            ; Frame Height Register                0x8005, 0xEF   --> FHR  = 240 pixels
JSR :__data_out
LDA #$28            ; Horizontal Address Range 0           0x8006
JSR :__data_out
LDA #$00            ; Horizontal Address Range 1           0x8007, 0x0028 --> HAR  =  40 addresses
JSR :__data_out

LDA #$44            ; Sets screen block start addresses and sizes
JSR :__comm_out
LDA #$00            ; Screen Block 1 Start Address 0       0x800b
JSR :__data_out
LDA #$00            ; Screen Block 1 Start Address 1       0x800c, 0x0000 --> SB1A = 0x0000
JSR :__data_out
LDA #$EF            ; Screen Block 1 Size                  0x800d, 0xef   --> SB1S = 240 pixels
JSR :__data_out
LDA #$B0            ; Screen Block 2 Start Address 0       0x800e
JSR :__data_out
LDA #$04            ; Screen Block 2 Start Address 1       0x800f, 0x04b0 --> SB2A = 0x04b0
JSR :__data_out
LDA #$EF            ; Screen Block 2 Size                  0x8010, 0xef   --> SB2S = 240 pixels
JSR :__data_out
LDA #$00            ; Screen Block 3 Start Address 0       0x8011
JSR :__data_out
LDA #$00            ; Screen Block 3 Start Address 1       0x8012, 0x0000 --> SB3A = 0x0000
JSR :__data_out
LDA #$00            ; Screen Block 4 Start Address 0       0x8013
JSR :__data_out
LDA #$00            ; Screen Block 4 Start Address 1       0x8014, 0x0000 --> SB4A = 0x0000
JSR :__data_out

LDA #$5A            ; Sets horizontal scroll position
JSR :__comm_out
LDA #$00            ; Horizontal Pixel Scroll              0x801b, 0x00   --> HPS  =   0  
JSR :__data_out

LDA #$5B            ; Sets display overlay format
JSR :__comm_out
LDA #$00            ; Overlay                              0x8018, 0x00   --> two layers, SB3 is text, SB1 is text, layers are OR'd
JSR :__data_out

LDA #$5D            ; Sets Cursor Type
JSR :__comm_out
LDA #$04            ; Cursor Width                         0x8015, 0x04   --> CW   =   4
JSR :__data_out
LDA #$86            ; Cursor Height                        0x8016, 0x86   --> CH   =   6, mode = block
JSR :__data_out

LDA #$4C            ; Sets direction of cursor movement,   0x8017, 0x4c   --> bits 00 --> Direction Right
JSR :__comm_out

LDA #$58            ; Enable / disable display
JSR :__comm_out
LDA #$01            ; Display Enable                       0x8009, 0x01   --> DE   = 1
JSR :__data_out

LDA #$59            ; Display attributes
JSR :__comm_out
LDA #$55            ; Display Attribute                    0x800a, 0x55   --> SAD3 ON, No flash; SAD2 ON, No Flash, SAD1 ON, No Flash, Cursor ON, No Flash
JSR :__data_out




LDA #$01        ; Display is initialised
STA $0201
RTS


:__comm_out
STA $4000       ; Data = ACCU
LDA #$07        ; WR=1 (read), RS=1 (cmd), RES=1 (not reset)
STA $4001
DEC $4001
INC $4001       ; WR=0 --> WR=1
RTS

:__data_out
STA $4000       ; Data = ACCU
LDA #$05        ; WR=1 (read), RS=0 (data), RES=1 (not reset)
STA $4001
DEC $4001
INC $4001       ; WR=0 --> WR=1
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
