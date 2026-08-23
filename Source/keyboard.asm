.orig $8000

; The '~' scan codes are special keys (TBD)
; $00 means 'unused scan code'
scancodes = [ 
            ; 00-09  DEL, RET, LR,  F1,  F3,  F5,  F7,  UD
                $00, '~', '~', '~', '~', '~', '~', '~', '~', $00,
            ; 10-19                                     SFT
                $00, '3', 'W', 'A', 'Z', 'S', 'E', '4', '~', $00,
            ; 20-29
                $00, '5', 'R', 'D', 'C', 'F', 'T', '6', 'X', $00, 
            ; 30-39
                $00, '7', 'Y', 'G', 'B', 'H', 'U', '8', 'V', $00, 
            ; 40-49
                $00, '9', 'I', 'J', 'M', 'K', 'O', '0', 'N', $00,
            ; 50-59
                $00, '+', 'P', 'L', '>', ':', '@', '-', '<', $00,
            ; 60-69                 SFT            CLR
                $00, '$', '*', ';', '~', '=', '^', '~', '/', $00,
            ; 70-79            CTR                      STP
                $00, '1', '<', '~', $00, 'C', 'Q', '2', '~', $00
            ]

; function call parameters
par1 = $80
par2 = $81

; video, character mode addresses
vchar_row = $94
vchar_col = $95

; 6522 registers
ddra = $4307
porta = $4107
ddrb = $4207
portb = $4007

; program vars
welcome = "Keyboard Test Program"
scanmask = $c0
scancode_hi = $c1
scancode_lo = $c2

; init
SEI             ; disable interrupts for startup
CLD             ; set CPU in decimal mode
LDX #$ff        ; initialise stack
TXS

; init 6522
LDA #$ff
STA ddra        ; porta as output (1=out)
STA porta       ; set all bits to 1: we scan active low
LDA #$00
STA ddrb        ; portb as input

; init lcd
JSR VIO_ResetDisplay
JSR VIO_InitDisplay
JSR VIO_ClearDisplay
JSR InitVideoRam

CLI             ; enable interrupts again: startup is done

program_init:
LDA #$01        ; set_cursor(1,1)
STA vchar_col
STA vchar_row
JSR CalcCharPtr

LDA #<welcome   ; print_string(welcome)
STA par1
LDA #>welcome
STA par2
JSR WriteString

BRK

program_loop:

keyb_start:
LDA #$fe
STA scanmask    ; initialise mask
LDX #$00

keyb_loop:
    ; keyboard loop uses X for out, Y for in

    LDA scanmask            ; porta = scanmask
    STA porta

    LDY #$08
    LDA portb
    keyb_read_loop:
        ASL                 ; rotate left with a zero
        BCC key_pressed     ; if a zero 'falls out' we have a key pressed

        DEY
        BEQ end_keyb_read_loop    ; inner loop done
        JMP keyb_read_loop
    end_keyb_read_loop:    

    INX

    LDA scanmask            ; ACC = scanmask << 1
    SEC                     
    ROL

    CMP #$ff                ; if ACC == $ff
    BEQ keyb_done           ; -> we are done

    STA scanmask            ; else: scanmask = ACC
    JMP keyb_loop

key_pressed:
    STX scancode_lo
    STY scancode_hi

    LDA #$02                ; set_cursor(2,1)
    STA vchar_row
    LDA #$01        
    STA vchar_col
    JSR CalcCharPtr

    LDA scancode_lo         ; print("%d", scancode_lo)
    CLC
    ADC '0'
    JSR WriteChar
    LDA scancode_hi         ; print("%d", scancode_lo)
    CLC
    ADC '0'
    JSR WriteChar

keyb_done:


JSR VIO_WriteCharScreen     ; write out video ram to screen
JMP program_loop
