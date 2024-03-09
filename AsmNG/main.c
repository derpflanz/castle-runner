#include <stdio.h>
#include "generated/parser.h"

int yylex(void);
extern char *yytext;
extern int linecounter;
int errors = 0;

void yyerror(char *s) {
    printf("%s in line %d.\n", s, linecounter);
    errors++;
}

int main(int argc, char **argv) {
    yyparse();

    if (errors > 0) {
        printf("Parsing failed with %d error(s).\n", errors);
        return 1;
    }

    return 0;
}