%{
#include "parser.h"
#include <ctype.h>
int lexlinecounter = 1;

%}
%%


(?xi:
	ADC|AND|ASL|BCC|BCS|BEQ|BIT|BMI|BNE|BPL|BRK|
	BVC|BVS|CLC|CLD|CLI|CLV|CMP|CPX|CPY|DEC|DEX|
	DEY|EOR|INC|INX|INY|JMP|JSR|LDA|LDX|LDY|LSR|
	NOP|ORA|PHA|PHP|PLA|PLP|ROL|ROR|RTI|RTS|SBC|
	SEC|SED|SEI|STA|STX|STY|TAX|TAY|TSX|TXA|TXS|
	TYA
) {
    yylval.str = strdup(yytext);
	for(char *ptr = yylval.str; *ptr = tolower(*ptr); *ptr++);

	return MNEMONIC;
}

[a-zA-Z_][a-zA-Z0-9_]+ {
	yylval.str = strdup(yytext);
	return IDENTIFIER;
}

\.(orig) {
	yylval.str = strdup(yytext);
	return DIRECTIVE;
}

\$[0-9a-fA-F]{4} {
	yylval.addr = strtol(yytext + 1, NULL, 16);
	return ABSOLUTE;
}

\$[0-9a-fA-F]{2} {
	yylval.addr = strtol(yytext + 1, NULL, 16);
	return ZEROPAGE;
}


,[Xx] {
	return COMMAX;
}

\n {
	lexlinecounter++;
	return *yytext;
}

[\(\)=A] {
	return *yytext;
}

;.*$ 			;
[ \t]+          ;

.    printf("Lexer error: invalid token on line %d: %s\n", lexlinecounter, yytext);

