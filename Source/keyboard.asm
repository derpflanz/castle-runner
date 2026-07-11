.orig $8000

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

LDA #$fe
STA scanmask    ; initialise mask


program_loop:
LDA scanmask

SEC                     ; rotate left with a one
ROL
CMP #$ff                ; if ACC == $ff
BNE _endif_acc_is_ff
LDA #$fe                ; reset to $fe
_endif_acc_is_ff:

STA scanmask            ; store new mask
STA porta

JSR VIO_WriteCharScreen     ; write out video ram to screen
JMP program_loop
