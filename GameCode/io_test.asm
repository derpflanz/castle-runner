@TEST "Dit is een test"

SEI             ; 78
CLD             ; Clear "D" flag: use binary mode (instead of BCD)
LDX #$ff        ; Initialise stack on 0x01ff
TXS
CLI


LDA #$4f            ; cursor direction down
JSR :__vio_comm_out
LDA #$B0            ; cursor ($40) = $04b0  (start of graph memory in screen)
STA $40
LDA #$04
STA $41
LDA #$b0            ; block_begin ($42) = 0x06b0 (start of graph memory in ram)
STA $42
LDA #$06
STA $43
LDA #$00            ; col = 0
STA $46

:__wps_start_col
LDA $40
STA $80
LDA $41
STA $81
JSR :__vio_set_cursor
LDA $42            ; block_end = block_begin
STA $44
LDA $43
STA $45
CLC
LDA $44           ; block_end = block_end + $ef   ($ef=239=last line of screen)
ADC #$EF
STA $44
LDA $45
ADC #$00
STA $45

LDA $42            ; __vio_write_memblock(block_begin, block_end)
STA $80
LDA $43
STA $81
LDA $44            ; $ff minus (LSB of end address) == LSB XOR $FF
EOR #$FF
STA $82
LDA $45
STA $83
JSR :__vio_write_memblock


:__vio_comm_out
BRK
NOP
RTS

:__vio_set_cursor
BRK
NOP
RTS

:__vio_write_memblock
BRK
NOP
RTS

:end
BRK

:HW_IRQ
RTI