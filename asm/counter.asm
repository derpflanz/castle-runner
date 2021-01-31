

;LDA	#	A9	A9 01
LDA #$1

;STA	zp	85	85 00			
STA $0

;LDA	zp	A5	A5 00			
LDA $0

;STA	a	AD	8D FA 7F			
STA $FA7F

;INC	zp	E6	E6 00			
;JMP	a	4C	4C 04 80
