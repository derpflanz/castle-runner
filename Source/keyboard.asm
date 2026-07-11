.orig $8000

; function call parameters
par1 = $80
par2 = $81

; 6522 registers
ddra = $4307
porta = $4107
ddrb = $4207
portb = $4007

welcome = "Keyboard Test Program"

; init
SEI
CLD
LDX #$ff
TXS

; init 6522
LDA #$ff
STA ddra        ; porta as output
STA porta       ; set all bits to 1: we scan active low
LDA #$00
STA ddrb        ; portb as input


; init lcd
JSR VIO_ResetDisplay
JSR VIO_InitDisplay
JSR VIO_ClearDisplay
JSR InitVideoRam



LDA #$00
STA $0200

CLI

program_init:
LDA #$01
STA $94
STA $95
JSR CalcCharPtr

LDA #<welcome
STA par1
LDA #>welcome
STA par2
JSR WriteString

program_loop:



; write out video ram to screen
JSR VIO_WriteCharScreen
JMP program_loop
