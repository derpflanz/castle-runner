; counter with read/write to zero page
; and write to hard wired output (address 0x7ffa)

LDA #$1                 ; accu = 1
STA $0                  ; mem[0] = accu
; start loop
:loop1
LDA $0                  ; accu = mem[0]
STA $7FFa               ; mem[7ffa] = accu
INC $0                  ; mem[0]++
JMP $8004               ; goto 8004
