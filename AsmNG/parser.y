%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../identifier.h"
#include "../opcode.h"
#include "../tree.h"
#include "parser.h"

#define ERRBUFLEN 1024

int yylex(void);
void yyerror(char *);

int linecounter = 1;
unsigned short current_address = 0x0000;
char error_msg[ERRBUFLEN];
struct operand NULL_ADDR = { NULL, 0, '\0' };

void directive(char *directive, struct operand operand) {
    if (!strncmp("orig", directive, 4)) {
        current_address = strtol((operand.str)+1, NULL, 16);
    }

    if (!strncmp("byte", directive, 4)) {
        tree_add_byte(operand.str);
        free(operand.str);
        current_address++;
    }

    free(directive);
}

void statement(char *mnemonic, struct operand operand, const char *addressing_mode) {
    current_address += get_statement_length(addressing_mode);

    tree_add_opcode(mnemonic, operand, addressing_mode);

    if (operand.str != NULL) free(operand.str);
    free(mnemonic);
}

// the string includes " and zero terminating 
void string(char *s) {
    s[strlen(s)-1] = '\0';
    current_address += strlen(s) - 2;    

    tree_add_string(s);
    free(s);
}

bool is_zp(struct operand operand) {
    if (operand.operation == '>' || operand.operation == '<') {
        return true;
    }

    // operand is zero page if:
    // - the operand looks like $xx, OR
    // - the operand is an identifier and its address is $xx
    if (operand.str[0] == '$' && isxdigit(operand.str[1]) && isxdigit(operand.str[2]) && operand.str[3] == '\0') {
        return true;
    }

    unsigned short address = 0x0000;
    if (get_address(operand.str, &address)) {
        if ((address | 0x00ff) == 0x00ff) {
            return true;
        }
    }

    // if it is not in the identifiers list, then it can only be a label (which is never zp)
    return false;
}

void identifier(char *ident, unsigned short addr) {
    identifier_add(ident, addr);
}

%}

%code requires {
    struct operand {
        char *str;
        int offset;
        char operation;
    };
}

%token MNEMONIC
%token BRANCH_MNEMONIC
%token ABSOLUTE
%token IDENTIFIER
%token DIRECTIVE
%token ZEROPAGE
%token NUMBER
%token OPERATION
%token STRING

%union {
    char *str;
    int number;
    struct operand ao;
    char ch;
}

%type<str> MNEMONIC
%type<str> BRANCH_MNEMONIC
%type<str> DIRECTIVE
%type<str> IDENTIFIER
%type<str> ABSOLUTE
%type<str> ZEROPAGE
%type<str> STRING
%type<number> NUMBER
%type<ch> OPERATION
%type<ao> zp_abs_identifier
%type<ao> zp_abs
%type<ao> zp_identifier
%type<ao> abs_identifier
%type<ao> abs
%type<ao> zp
%type<ao> ident
%type<ao> ident_oper
%%

program:
    program expression '\n' { linecounter++; }
|   program '\n'            { linecounter++; }
|	program error '\n'		{ yyerrok; }
|   /* NOTHING */
;

expression:
    MNEMONIC                                { statement($1, NULL_ADDR,  "i"); }
|   MNEMONIC accu                           { statement($1, NULL_ADDR,  "i"); }
|   MNEMONIC '#' zp_identifier              { statement($1, $3,         "#"); }
|   MNEMONIC zp_abs_identifier ',' x        { statement($1, $2,         is_zp($2)?"zp,x":"a,x"); }
|   MNEMONIC zp_abs_identifier ',' y        { statement($1, $2,         is_zp($2)?"zp,y":"a,y"); }
|   MNEMONIC zp_abs_identifier              { statement($1, $2,         is_zp($2)?"zp":"a"); }
|   MNEMONIC '(' zp_identifier ')'          { statement($1, $3,         is_zp($3)?"(zp)":"(a)"); }
|   MNEMONIC '(' abs ')'                    { statement($1, $3,         "(a)"); }
|   MNEMONIC '(' zp_identifier ')' ',' y    { statement($1, $3,         "(zp),y"); }
|   MNEMONIC '(' zp_identifier ',' x ')'    { statement($1, $3,         "(zp,x)"); }
|   BRANCH_MNEMONIC abs_identifier          { statement($1, $2,         "r"); }
|   IDENTIFIER '=' zp_abs                   { identifier($1, strtol(($3.str)+1, NULL, 16)); }
|   IDENTIFIER '=' STRING                   { identifier($1, current_address); string($3); }
|   IDENTIFIER ':'                          { identifier($1, current_address); }
|   DIRECTIVE zp_abs                        { directive($1, $2); }
;

x: 'X' | 'x' ;
y: 'Y' | 'y' ;
accu: 'A' | 'a';

abs:
    ABSOLUTE { $$.str = $1; }
;

zp:
    ZEROPAGE { $$.str = $1; }
|   OPERATION ABSOLUTE { $$.str = $2; $$.operation = $1; }
;

ident:
    IDENTIFIER { $$.str = $1; }
|   IDENTIFIER '+' NUMBER { $$.str = $1; $$.offset = $3; }
|   IDENTIFIER '-' NUMBER { $$.str = $1; $$.offset = -$3; }
;

ident_oper:
    ident
|   OPERATION ident { $$.operation = $1; $$.str = $2.str; $$.offset = $2.offset; }
;

zp_identifier:
    zp
|   ident_oper
;

zp_abs:
    zp
|   abs
;

abs_identifier:
    abs
|   ident
;

zp_abs_identifier:
    zp
|   abs
|   ident_oper
;

%%

