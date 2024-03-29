%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../identifier.h"
#include "parser.h"
int yylex(void);
void yyerror(char *);

#define OPCODE_COUNT 256
#define ERRBUFLEN 1024

int linecounter = 1;
unsigned short address = 0x0000;
char error_msg[ERRBUFLEN];

struct addr_offset NULL_ADDR = { NULL, 0, '\0' };

static const char *mnemonics[OPCODE_COUNT] = {
/*         |  0    |  1    |  2    |  3 |  4    |  5    |  6    |  7     |  8    |  9    |  A    |  B  |    C  |    D  |    E    |  F    |      */
/* 0 */      "brk",  "ora",  "",     "",  "tsb",  "ora",  "asl",  "rmb0",  "php",  "ora",  "asl",  "",     "tsb",  "ora",  "asl",  "bbr0", /* 0 */
/* 1 */      "bpl",  "ora",  "ora",  "",  "trb",  "ora",  "asl",  "rmb1",  "clc",  "ora",  "inc",  "",     "trb",  "ora",  "asl",  "bbr1", /* 1 */
/* 2 */      "jsr",  "and",  "",     "",  "bit",  "and",  "rol",  "rmb2",  "plp",  "and",  "rol",  "",     "bit",  "and",  "rol",  "bbr2", /* 2 */
/* 3 */      "bmi",  "and",  "and",  "",  "bit",  "and",  "rol",  "rmb3",  "sec",  "and",  "dec",  "",     "bit",  "and",  "rol",  "bbr3", /* 3 */
/* 4 */      "rti",  "eor",  "",     "",  "",     "eor",  "lsr",  "rmb4",  "pha",  "eor",  "lsr",  "",     "jmp",  "eor",  "lsr",  "bbr4", /* 4 */
/* 5 */      "bvc",  "eor",  "eor",  "",  "",     "eor",  "lsr",  "rmb5",  "cli",  "eor",  "phy",  "",     "",     "eor",  "lsr",  "bbr5", /* 5 */
/* 6 */      "rts",  "adc",  "",     "",  "stz",  "adc",  "ror",  "rmb6",  "pla",  "adc",  "ror",  "",     "jmp",  "adc",  "ror",  "bbr6", /* 6 */
/* 7 */      "bvs",  "adc",  "adc",  "",  "stz",  "adc",  "ror",  "rmb7",  "sei",  "adc",  "ply",  "",     "jmp",  "adc",  "ror",  "bbr7", /* 7 */
/* 8 */      "bra",  "sta",  "",     "",  "sty",  "sta",  "stx",  "smb0",  "dey",  "bit",  "txa",  "",     "sty",  "sta",  "stx",  "bbs0", /* 8 */
/* 9 */      "bcc",  "sta",  "sta",  "",  "sty",  "sta",  "stx",  "smb1",  "tya",  "sta",  "txs",  "",     "stz",  "sta",  "stz",  "bbs1", /* 9 */
/* A */      "ldy",  "lda",  "ldx",  "",  "ldy",  "lda",  "ldx",  "smb2",  "tay",  "lda",  "tax",  "",     "ldy",  "lda",  "ldx",  "bbs2", /* A */
/* B */      "bcs",  "lda",  "lda",  "",  "ldy",  "lda",  "ldx",  "smb3",  "clv",  "lda",  "tsx",  "",     "ldy",  "lda",  "ldx",  "bbs3", /* B */
/* C */      "cpy",  "cmp",  "",     "",  "cpy",  "cmp",  "dec",  "smb4",  "iny",  "cmp",  "dex",  "wai",  "cpy",  "cmp",  "dec",  "bbs4", /* C */
/* D */      "bne",  "cmp",  "cmp",  "",  "",     "cmp",  "dec",  "smb5",  "cld",  "cmp",  "phx",  "stp",  "",     "cmp",  "dec",  "bbs5", /* D */
/* E */      "cpx",  "sbc",  "",     "",  "cpx",  "sbc",  "inc",  "smb6",  "inx",  "sbc",  "nop",  "",     "cpx",  "sbc",  "inc",  "bbs6", /* E */
/* F */      "beq",  "sbc",  "sbc",  "",  "",     "sbc",  "inc",  "smb7",  "sed",  "sbc",  "plx",  "",     "",     "sbc",  "inc",  "bbs7"  /* F */
};


// a,x     Absolute Indexed with X (3)                 ex. $7fe4,X           
// a,y     Absolute Indexed with Y (4)                 ex. $7fe4,Y           
// (a,x)   Absolute Indexed Indirect (2)               ex. ($4fe4,x)         
// (a)     Absolute Indirect (5)                       ex. ($1000)           
// a       Absolute (1)                                ex. $4fe4             
// #       Immediate (7)                               ex. #$1 or LO($8000)  
// zp,x    Zero Page Indexed with X (13)               ex. $7f,x             
// zp,y    Zero Page Indexed with Y (14)               ex. $7f,y             
// zp      Zero Page (11)                              ex. $7f               
// (zp,x)  Zero Page Indexed Indirect (12)             ex. ($7f,x)           
// (zp),y  Zero Page Indirect Indexed with Y (16)      ex. ($10),Y           
// (zp)    Zero Page Indirect (15)                     ex. ($7f)
// r       Program Counter Relative (9)                ex. $04
// the below adressing modes are all "i" in the list below
// A       Accumulator (6)
// i       Implied (8)
// s       Stack (10)

// Note on the addressing mode table:
// all addressing modes without an operand, are labeled 'i': implied
// this goes for the 's' (stack) and 'A' (accu) modes
static char *addressing_modes[OPCODE_COUNT] = {
/*        |  0  |  1       |  2    |  3  |  4     |  5     |  6     |  7     |  8  |  9    |  A  |  B  |  C   |    D   |  E   |  F  |     */
/* 0 */     "i",  "(zp,x)",  "",     "",   "zp",    "zp",    "zp",    "zp",    "i",  "#",    "i",  "",   "a",     "a",   "a",   "r", /* 0 */
/* 1 */     "r",  "(zp),y",  "(zp)", "",   "zp",    "zp,x",  "zp,x",  "zp",    "i",  "a,y",  "i",  "",   "a",     "a,x", "a,x", "r", /* 1 */
/* 2 */     "a",  "(zp,x)",  "",     "",   "zp",    "zp",    "zp",    "zp",    "i",  "#",    "i",  "",   "a",     "a",   "a",   "r", /* 2 */
/* 3 */     "r",  "(zp),y",  "(zp)", "",   "zp,x",  "zp,x",  "zp,x",  "zp",    "i",  "a,y",  "i",  "",   "a,x",   "a,x", "a,x", "r", /* 3 */
/* 4 */     "i",  "(zp,x)",  "",     "",   "",      "zp",    "zp",    "zp",    "i",  "#",    "i",  "",   "a",     "a",   "a",   "r", /* 4 */
/* 5 */     "r",  "(zp),y",  "(zp)", "",   "",      "zp,x",  "zp,x",  "zp",    "i",  "a,y",  "i",  "",   "",      "a,x", "a,x", "r", /* 5 */
/* 6 */     "i",  "(zp,x)",  "",     "",   "zp",    "zp",    "zp",    "zp",    "i",  "#",    "i",  "",   "(a)",   "a",   "a",   "r", /* 6 */
/* 7 */     "r",  "(zp),y",  "(zp)", "",   "zp,x",  "zp,x",  "zp,x",  "zp",    "i",  "a,y",  "i",  "",   "(a,x)", "a,x", "a,x", "r", /* 7 */
/* 8 */     "r",  "(zp,x)",  "",     "",   "zp",    "zp",    "zp",    "zp",    "i",  "#",    "i",  "",   "a",     "a",   "a",   "r", /* 8 */
/* 9 */     "r",  "(zp),y",  "(zp)", "",   "zp,x",  "zp,x",  "zp,y",  "zp",    "i",  "a,y",  "i",  "",   "a",     "a,x", "a,x", "r", /* 9 */
/* A */     "#",  "(zp,x)",  "#",    "",   "zp",    "zp",    "zp",    "zp",    "i",  "#",    "i",  "",   "a",     "a",   "a",   "r", /* A */
/* B */     "r",  "(zp),y",  "(zp)", "",   "zp,x",  "zp,x",  "zp,y",  "zp",    "i",  "a,y",  "i",  "",   "a,x",   "a,x", "a,y", "r", /* B */
/* C */     "#",  "(zp,x)",  "",     "",   "zp",    "zp",    "zp",    "zp",    "i",  "#",    "i",  "i",  "a",     "a",   "a",   "r", /* C */
/* D */     "r",  "(zp),y",  "(zp)", "",   "",      "zp,x",  "zp,x",  "zp",    "i",  "a,y",  "i",  "i",  "",      "a,x", "a,x", "r", /* D */
/* E */     "#",  "(zp,x)",  "",     "",   "zp",    "zp",    "zp",    "zp",    "i",  "#",    "i",  "",   "a",     "a",   "a",   "r", /* E */
/* F */     "r",  "(zp),y",  "(zp)", "",   "",      "zp,x",  "zp,x",  "zp",    "i",  "a,y",  "i",  "",   "",      "a,x", "a,x", "r"  /* F */
};


bool opcode_lookup(const char *mnemonic, const char *mode, unsigned char *opcode) {
    for (unsigned char i = 0; i < OPCODE_COUNT-1; i++) {
        if ((!strncmp(mnemonics[i],         mnemonic,   strlen(mnemonic))) &&
            (!strncmp(addressing_modes[i],  mode,       strlen(mode)))
        ) {
            *opcode = i;
            return true;
        }
    }

    return false;
}

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

bool get_address(char *ident, unsigned short *address) {
    struct identifier *p = identifiers;

    while (p) {
        if (!strncmp(ident, p->name, strlen(ident))) {
            *address = p->address;
            return true;
        }
        p = p->next;
    }

    return false;
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

