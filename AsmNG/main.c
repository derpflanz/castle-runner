#include <stdio.h>
#include "generated/parser.h"

int yylex(void);
extern char *yytext;
extern int linecounter;

void yyerror(char *s) {
    printf("%s in line %d: %s\n", s, linecounter, yytext);
}

int main(int argc, char **argv) {
    yyparse();

    return 0;
}