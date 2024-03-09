%{
#include "parser.h"
int lexlinecounter = 1;

%}
%%

[a-zA-Z]{3} {
    yylval.str = strdup(yytext);
	return MNEMONIC;
}

[a-zA-Z_][a-zA-Z0-9_]+: {
	yylval.str = strdup(yytext);
	return IDENTIFIER;
}

\.(orig|byte) {
	yylval.str = strdup(yytext);
	return DIRECTIVE;
}

\$[0-9a-fA-F]{4} {
	yylval.addr = strtol(yytext + 1, NULL, 16);
	return ADDRESS;
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

[\(\)] {
	return *yytext;
}

[ \t]+          ;

.    printf("Invalid token in line %d: %s\n", lexlinecounter, yytext);

