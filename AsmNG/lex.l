%{
#include "parser.h"
#include <ctype.h>
int lexlinecounter = 1;
int lexerrorcounter = 0;

%}
%%


(?xi:
	ADC|AND|ASL|BIT|BRK|CLC|CLD|CLI|CLV|CMP|CPX|
	DEY|EOR|INC|INX|INY|JMP|JSR|LDA|LDX|LDY|LSR|
	NOP|ORA|PHA|PHP|PLA|PLP|ROL|ROR|RTI|RTS|SBC|
	SEC|SED|SEI|STA|STX|STY|TAX|TAY|TSX|TXA|TXS|
	TYA|CPY|DEC|DEX
) {
    yylval.str = strdup(yytext);
	for(char *ptr = yylval.str; *ptr = tolower(*ptr); *ptr++);

	return MNEMONIC;
}

(?xi:
	BCC|BCS|BEQ|BMI|BNE|BPL|BVC|BVS|BRA|BB[RS][0-9]
) {
	yylval.str = strdup(yytext);
	for(char *ptr = yylval.str; *ptr = tolower(*ptr); *ptr++);

	return BRANCH_MNEMONIC;
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
	yylval.str = strdup(yytext);
	return ABSOLUTE;
}

[0-9]+ {
	yylval.number = strtol(yytext, NULL, 10);
	return NUMBER;
}

\$[0-9a-fA-F]{2} {
	yylval.str = strdup(yytext);
	return ZEROPAGE;
}

\n {
	lexlinecounter++;
	return *yytext;
}

[\(\)=Aa#XxYy,:\+\-\<\>] {
	return *yytext;
}

;.*$ 			;
[ \t]+          ;

.    printf("Lexer error %d: invalid token on line %d: %s (%d)\n", ++lexerrorcounter, lexlinecounter, yytext);

