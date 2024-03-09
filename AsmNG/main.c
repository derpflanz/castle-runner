#include <stdio.h>
#include "generated/parser.h"

int yylex(void);
extern char *yytext;
extern int linecounter;
extern int lexerrorcounter;
int errors = 0;

void yyerror(char *s) {
    printf("%s in line %d.\n", s, linecounter);
    errors++;
}

int main(int argc, char **argv) {
    yyparse();

    if (errors + lexerrorcounter > 0) {
        printf("Parsing failed with %d error(s).\n", errors + lexerrorcounter);
        return 1;
    }

    return 0;
}