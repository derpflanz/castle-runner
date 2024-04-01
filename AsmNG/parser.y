%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../identifier.h"
#include "../opcode.h"
#include "parser.h"
int yylex(void);
void yyerror(char *);

#define ERRBUFLEN 1024

int linecounter = 1;
unsigned short address = 0x0000;
char error_msg[ERRBUFLEN];

struct addr_offset NULL_ADDR = { NULL, 0, '\0' };

void directive(char *directive, struct addr_offset ao) {    
    if (!strncmp("orig", directive, 4)) {
        address = strtol((ao.str)+1, NULL, 16);
    }

    if (!strncmp("byte", directive, 4)) {
        // add element to tree
        address++;
    }

    free(directive);
}



int get_statement_length(const char *addressing_mode) {
    if (addressing_mode[0] == 'i') return 1;
    if (strchr(addressing_mode, 'a') != NULL) return 3;

    return 2;
}

void statement(int callid, char *mnemonic, struct addr_offset ao, const char *addressing_mode) {
    char *operand = NULL;
    int offset = 0;
    operand = ao.str;
    offset = ao.offset;

    unsigned char opcode = 0x00;
    if (!opcode_lookup(mnemonic, addressing_mode, &opcode)) {
        snprintf(error_msg, ERRBUFLEN, "Opcode not found for mnemonic %s and addressing mode %s", mnemonic, addressing_mode);
        yyerror(error_msg);
    }

    printf("%02d [%04x] %d  mn: %s (%p), operand: %-7s (offset %4d)  mode=%-6s operation=%c %10s opcode: %02x (%s) +%d bytes\n", 
        callid, address, linecounter, mnemonic, mnemonic, operand, offset, addressing_mode, ao.operation, "", opcode, mnemonics[opcode], 
        get_statement_length(addressing_mode));

    address += get_statement_length(addressing_mode);

    if (operand != NULL) free(operand);
    free(mnemonic);

    // add element to tree
}

// the string includes " and zero terminating 
void string(char *s) {
    s[strlen(s)-1] = '\0';
    printf("String: %s\n", s+1);
    address += strlen(s-2);
    free(s);

    // add element to tree
}

bool is_zp(struct addr_offset ao) {
    char *operand = ao.str;

    if (ao.operation == '>' || ao.operation == '<') {
        return true;
    }

    // operand is zero page if:
    // - the operand looks like $xx, OR
    // - the operand is an identifier and its address is $xx
    if (operand[0] == '$' && isxdigit(operand[1]) && isxdigit(operand[2]) && operand[3] == '\0') {
        return true;
    }

    unsigned short address = 0x0000;
    if (get_address(operand, &address)) {
        if ((address | 0x00ff) == 0x00ff) {
            return true;
        }
    }

    // if it is not in the identifiers list, then it can only be a label (which is never zp)
    return false;
}

%}

%code requires {
    struct addr_offset {
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
    struct addr_offset ao;
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

%%

program:
    program expression '\n' { linecounter++; }
|   program '\n'            { linecounter++; }
|	program error '\n'		{ yyerrok; }
|   /* NOTHING */
;

expression:
    MNEMONIC                                { statement(0,  $1, NULL_ADDR,   "i"); }
|   MNEMONIC accu                           { statement(1,  $1, NULL_ADDR,   "i"); }
|   MNEMONIC '#' zp_identifier              { statement(2,  $1, $3,     "#"); }
|   MNEMONIC zp_abs_identifier ',' x        { statement(3,  $1, $2,     is_zp($2)?"zp,x":"a,x"); }
|   MNEMONIC zp_abs_identifier ',' y        { statement(4,  $1, $2,     is_zp($2)?"zp,y":"a,y"); }
|   MNEMONIC zp_abs_identifier              { statement(5,  $1, $2,     is_zp($2)?"zp":"a"); }
|   MNEMONIC '(' zp_identifier ')'          { statement(6,  $1, $3,     is_zp($3)?"(zp)":"(a)"); }
|   MNEMONIC '(' abs ')'                    { statement(7,  $1, $3,     "(a)"); }
|   MNEMONIC '(' zp_identifier ')' ',' y    { statement(8,  $1, $3,     "(zp),y"); }
|   MNEMONIC '(' zp_identifier ',' x ')'    { statement(9,  $1, $3,     "(zp,x)"); }
|   BRANCH_MNEMONIC abs_identifier          { statement(10, $1, $2,     "r"); }
|   IDENTIFIER '=' zp_abs                   { identifiers = register_identifier(identifiers, $1, strtol(($3.str)+1, NULL, 16)); }
|   IDENTIFIER '=' STRING                   { identifiers = register_identifier(identifiers, $1, address); string($3); }
|   IDENTIFIER ':'                          { identifiers = register_identifier(identifiers, $1, address); }
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
|   OPERATION IDENTIFIER { $$.str = $2; $$.operation = $1; }
;

zp_identifier:
    zp
|   ident
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
|   ident
;

%%

