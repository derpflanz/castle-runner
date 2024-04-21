#include <stdio.h>
#include "generated/parser.h"
#include "identifier.h"
#include "tree.h"

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

    identifier_print();

    tree_print();

    return 0;
}