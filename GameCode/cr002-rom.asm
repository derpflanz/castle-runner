; This is the CR002 ROM
; 
; This ROM provides routines to use the CR002 computer
; - Video now runs in an interrupt and is 320x240 pixels big
;
; Generic
; $40 - $7f     - used as local variables in function calls
; $80 - $8F     - used for parameters in function calls
; $90 - $BF     - used for video pointers
;
; Memory map:
; VIDEO  0x0200 - 0x2FFF reserved for video related stuff (12kB)
;        0x0200 - 0x06AF Character display
;        0x06B0 - 0x2C30 Graphics display
;          0x90 -   0x91 Character cursor pointer (little endian, so we can use ($90),Y to address), range is 0x0200-0x06AF
;          0x92 -   0x93 Graphics video pointer (little endian), range is 0x06B0-0x2C30
;
;
; OUTPUT 0x4000 is display data (D0-D7)
;        0x4001 is display control:
;                   bit0: /WR
;                   bit1: RS (1=cmd, 0=data)
;                   bit2: /RESET
;
; Video call that start with VIO_ do actual video IO

:VIO_ResetDisplay
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

:VIO_InitDisplay
    ; Display initialisation is a whole sequence of bytes
    LDA #$40            ; Initializes device and display
    JSR :__vio_comm_out
    LDA #$30            ; Memory Configuration Register        0x8000, 0x03   --> Config: Origin Comp = 1, Single Panel, Char Height = 8 pixels, CGROM selected
    JSR :__vio_data_out
    LDA #$87            ; Horizontal Character Size            0x8001, 0x07   --> HCS  =   8
    JSR :__vio_data_out
    LDA #$07            ; Vertical Character Size              0x8002, 0x07   --> VCS  =   8
    JSR :__vio_data_out
    LDA #$27            ; Character Bytes Per Row              0x8003, 0x27   --> C/R  =  40 chars
    JSR :__vio_data_out
    LDA #$50            ; Total Character Bytes Per Row        0x8004, 0x50   --> TC/R =  79
    JSR :__vio_data_out
    LDA #$EF            ; Frame Height Register                0x8005, 0xEF   --> FHR  = 240 pixels
    JSR :__vio_data_out
    LDA #$28            ; Horizontal Address Range 0           0x8006
    JSR :__vio_data_out
    LDA #$00            ; Horizontal Address Range 1           0x8007, 0x0028 --> HAR  =  40 addresses
    JSR :__vio_data_out
    LDA #$44            ; Sets screen block start addresses and sizes
    JSR :__vio_comm_out
    LDA #$00            ; Screen Block 1 Start Address 0       0x800b
    JSR :__vio_data_out
    LDA #$00            ; Screen Block 1 Start Address 1       0x800c, 0x0000 --> SB1A = 0x0000
    JSR :__vio_data_out
    LDA #$EF            ; Screen Block 1 Size                  0x800d, 0xef   --> SB1S = 240 pixels
    JSR :__vio_data_out
    LDA #$B0            ; Screen Block 2 Start Address 0       0x800e
    JSR :__vio_data_out
    LDA #$04            ; Screen Block 2 Start Address 1       0x800f, 0x04b0 --> SB2A = 0x04b0
    JSR :__vio_data_out
    LDA #$EF            ; Screen Block 2 Size                  0x8010, 0xef   --> SB2S = 240 pixels
    JSR :__vio_data_out
    LDA #$00            ; Screen Block 3 Start Address 0       0x8011
    JSR :__vio_data_out
    LDA #$00            ; Screen Block 3 Start Address 1       0x8012, 0x0000 --> SB3A = 0x0000
    JSR :__vio_data_out
    LDA #$00            ; Screen Block 4 Start Address 0       0x8013
    JSR :__vio_data_out
    LDA #$00            ; Screen Block 4 Start Address 1       0x8014, 0x0000 --> SB4A = 0x0000
    JSR :__vio_data_out
    LDA #$5A            ; Sets horizontal scroll position
    JSR :__vio_comm_out
    LDA #$00            ; Horizontal Pixel Scroll              0x801b, 0x00   --> HPS  =   0  
    JSR :__vio_data_out
    LDA #$5B            ; Sets display overlay format
    JSR :__vio_comm_out
    LDA #$00            ; Overlay                              0x8018, 0x00   --> two layers, SB3 is text, SB1 is text, layers are OR'd
    JSR :__vio_data_out
    LDA #$5D            ; Sets Cursor Type
    JSR :__vio_comm_out
    LDA #$04            ; Cursor Width                         0x8015, 0x04   --> CW   =   4
    JSR :__vio_data_out
    LDA #$86            ; Cursor Height                        0x8016, 0x86   --> CH   =   6, mode = block
    JSR :__vio_data_out
    LDA #$4C            ; Sets direction of cursor movement,   0x8017, 0x4c   --> bits 00 --> Direction Right
    JSR :__vio_comm_out
    LDA #$58            ; Enable / disable display
    JSR :__vio_comm_out
    LDA #$01            ; Display Enable                       0x8009, 0x01   --> DE   = 1
    JSR :__vio_data_out
    LDA #$59            ; Display attributes
    JSR :__vio_comm_out
    LDA #$55            ; Display Attribute                    0x800a, 0x55   --> SAD3 ON, No flash; SAD2 ON, No Flash, SAD1 ON, No Flash, Cursor ON, No Flash
    JSR :__vio_data_out
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
:VIO_ClearDisplay
    ; To clear the display, we write all 0 bytes
    LDA #$46            ; Set cursor to 0
    JSR :__vio_comm_out
    LDA #$00
    JSR :__vio_data_out
    LDA #$00
    JSR :__vio_data_out
    LDA #$42
    JSR :__vio_comm_out
    LDA #$6c            ; 0x6c x 0x64 = 108 x 100 = 10800 bytes
    STA $40
    :__clr_outer
    LDA #$64
    STA $41
    :__clr_inner
    LDA #$00
    JSR :__vio_data_out
    DEC $41
    BNE :__clr_inner
    DEC $40
    BNE :__clr_outer
RTS             ; ClearDisplay

; Writes $06B0 - $2C30 to display, starting at address 0x04b0
:VIO_WriteGraphScreen
    LDA #$B0
    STA $80
    LDA #$04
    STA $81
    JSR :__vio_set_cursor
    LDA #$b0            ; ($40) = $06b0 (start addr of graph video)
    STA $80
    LDA #$06
    STA $81
    LDA #$CF            ; $ff minus (LSB of end address) --> ($ff-$30=$cf)
    STA $82
    LDA #$2C            ; MSB of end address
    STA $83
    JSR :__vio_write_memblock
RTS

:__vio_write_memblock
    LDX $82             ; $82 = ($ff-LSB of end address)
    LDY #$00            ; y = 0
    :__wgs_loop
    LDA ($80),Y
    JSR :__vio_data_out
    CPY #$FF
    BNE :__wgs_incy
    INC $81
    :__wgs_incy
    INY
    INX
    CPX #$FF
    BNE :__wgs_loop
    LDA $81
    CMP $83            ; $83 = MSB of end address
    BNE :__wgs_loop
RTS                 ; WriteGraphScreen

; Writes $0200 - $06AF to display, starting at address 0
:VIO_WriteCharScreen
    LDA #$00            ; cursor = $0000
    STA $80
    STA $81
    JSR :__vio_set_cursor
    LDA #$00            ; ($40) = $0200 (start addr of char video)
    STA $40
    LDA #$02
    STA $41
    LDY #$00            ; y = 0
    LDX #$50            ; x = $ff - $af ($af is LSB of end address)
    :__wcs_loop
    LDA ($40),Y
    JSR :__vio_data_out
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

; WriteString
; Params: pointer to string in ($80)
; Result: the string is printed on screen at where ($90) was pointing at
:WriteString
    LDY #$ff        ; display counter (we start with an INY, so the BNE looks at ACCU)
    :__ws_loop
    INY
    LDA ($80),Y
    STA ($90),Y
    BNE :__ws_loop
RTS             ; WriteString

; GotoCharXY
; Params: LDX. LDY
; Result: The character screen pointer at ($90) is set to (x,y)
; Display is 40 character wide, 30 high
; Works in the video ram
:GotoCharXY
    STX $80
    STY $81
    LDA #$00        ; ($82) = $0200 + (40 * Y) + X
    STA $82
    LDA #$02
    STA $83
    JSR :__GotoXY
    LDA $82         ; ($90) = ($82)
    STA $90
    LDA $83
    STA $91
RTS             ; GotoCharXY

; GotoGraphXY
; Params: LDX. LDY
; Result: The graph screen pointer at ($92) is set to (x,y)
; Display is 40 character wide, 240 lines high
; Works in the video ram
:GotoGraphXY
    STX $80
    STY $81
    LDA #$B0        ; ($82) = $06B0 + (40 * Y) + X
    STA $82
    LDA #$06
    STA $83
    JSR :__GotoXY
    LDA $82         ; ($92) = ($82)
    STA $92
    LDA $83
    STA $93
RTS             ; GotoGraphXY

; WriteChar
; Writes character in ACCU into char video memory ($90)
:WriteChar
    LDY #$00
    STA ($90),Y
RTS

; WriteGraph
; Writes character in ACCU into graph video memory ($92)
:WriteGraph
    LDY #$00
    STA ($92),Y
RTS

;----------------------------------- INTERNALS -------------------------------------------------



; Generic internal goto XY
; Params: X=$80, Y=$81, Offset=($82), result will be in ($82)
; X can be 0 to 39
; Y can be 0 to 29 for char screen
; Y can be 0 to 239 for graph screen
; Works in video RAM
:__GotoXY
    LDA $81         ; ($90) += 40 * Y
    BEQ :__gxy_cols
    CLC
    LDA $82
    ADC #$28
    STA $82
    LDA $83
    ADC #$00
    STA $83
    DEC $81
    JMP :__GotoXY
    :__gxy_cols
    CLC
    LDA $82         ; ($90) += X
    ADC $80
    STA $82
    LDA $83
    ADC #$00
    STA $83
RTS             ; GotoCharXY

;--------- VIDEO IO CALLS -----------------------------
; prepended with __vio they do actual Video IO
:__vio_set_cursor
LDA #$46            ; Set cursor to 0
    JSR :__vio_comm_out
    LDA $80             ; LSB of internal display address (0000 = begin of char, 04b0 is begin of graph)
    JSR :__vio_data_out
    LDA $81
    JSR :__vio_data_out
    LDA #$42
    JSR :__vio_comm_out
RTS

:__vio_comm_out
    STA $4000       ; Data = ACCU
    LDA #$07        ; WR=1 (read), RS=1 (cmd), RES=1 (not reset)
    STA $4001
    DEC
    STA $4001
    INC
    STA $4001       ; WR=0 --> WR=1
RTS

:__vio_data_out
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
