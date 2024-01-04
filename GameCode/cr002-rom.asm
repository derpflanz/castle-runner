; This is the CR002 ROM
; 
; This ROM provides routines to use the CR002 computer
; - Video now runs in an interrupt and is 320x240 pixels big
;
; Generic
; $80 - $8F     - used for parameters in function calls
; $40 - $7f     - used as local variables in function calls
;
; Memory map:
; VIDEO  0x0200 - 0x2FFF reserved for video related stuff (12kB)
;        0x0200 - 0x06AF Character display
;        0x06B0 - 0x0C30 Graphics display
;
;
; OUTPUT 0x4000 is display data (D0-D7)
;        0x4001 is display control:
;                   bit0: /WR
;                   bit1: RS (1=cmd, 0=data)
;                   bit2: /RESET

:ResetDisplay
; To reset, we pull de /RES pin low for at least 3ms
LDA #$01        ; WR=1, RS=0, RES=0
STA $4001
LDA #$ff        ; number of full cycles to make $ff = 8ms
STA $40
:__reset_outer
LDA #$ff
:__reset_inner
DEC
BNE :__reset_inner
DEC $40
BNE :__reset_outer
LDA #$05        ; WR=1, RS=0, RES=1
STA $4001
RTS             ; ResetDisplay

:InitDisplay
; Display initialisation is a whole sequence of bytes
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
RTS                 ; InitDisplay

; InitVideoRam will zero all bytes between $0200 and $0C30
:InitVideoRam
LDA #$2C        ; MSB of $2C30 (top of video ram)
STA $41
LDA #$00
STA $40
LDY #$30        ; LSB of start addr $2C30 (MSB is in $40)
:__ivr_loop
LDA #$00
STA ($40),Y
CPY #$00
BNE :__ivr_decy
DEC $41
LDA $41
CMP #$01        ; MSB minus 1 of end address $0200
BEQ :__ivr_end
:__ivr_decy
DEY
JMP :__ivr_loop
:__ivr_end
RTS             ; InitVideoRam

; This might become deprecated if WriteCharScreen and WriteGraphScreen are finalised
:ClearDisplay
; To clear the display, we write all 0 bytes
LDA #$46            ; Set cursor to 0
JSR :__comm_out
LDA #$00
JSR :__data_out
LDA #$00
JSR :__data_out
LDA #$42
JSR :__comm_out
LDA #$6c            ; 0x6c x 0x64 = 108 x 100 = 10800 bytes
STA $40
:__clr_outer
LDA #$64
STA $41
:__clr_inner
LDA #$00
JSR :__data_out
DEC $41
BNE :__clr_inner
DEC $40
BNE :__clr_outer
RTS             ; ClearDisplay

; Writes $0200 - $06AF to display, starting at cursor 0
:WriteCharScreen
LDA #$46            ; Set cursor to 0
JSR :__comm_out
LDA #$00
JSR :__data_out
LDA #$00
JSR :__data_out
LDA #$42
JSR :__comm_out
LDA #$00            ; ($40) = $0200 (start addr of video)
STA $40
LDA #$02
STA $41
LDY #$00            ; y = 0
LDX #$50            ; x = $ff - $af ($af is LSB of end address)
:__wcs_loop
LDA ($40),Y
JSR :__data_out
CPY #$ff
BNE :__wcs_incy
INC $41
:__wcs_incy
INY
INX
CPX #$ff
BNE :__wcs_loop
LDA $41
CMP #$06            ; $06 = MSB of end address
BNE :__wcs_loop
RTS                 ; WriteCharScreen

; GotoCharXY
; Params: LDX, LDY
; Result: The character screen pointer is set to (x,y)
; Display is 40 character wide, 30 high
:GotoCharXY

RTS             ; GotoCharXY

:__comm_out
STA $4000       ; Data = ACCU
LDA #$07        ; WR=1 (read), RS=1 (cmd), RES=1 (not reset)
STA $4001
DEC
STA $4001
INC
STA $4001       ; WR=0 --> WR=1
RTS

:__data_out
STA $4000       ; Data = ACCU
LDA #$05        ; WR=1 (read), RS=0 (data), RES=1 (not reset)
STA $4001
DEC
STA $4001
INC
STA $4001       ; WR=0 --> WR=1
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
RTS             ; Dec2Ascii

; Below is actual IO, called from the IRQ handler
:HW_IRQ
SEI
CLI             ; Enable intterupts
RTI             ; And we're done
