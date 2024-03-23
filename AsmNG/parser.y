%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../identifier.h"
int yylex(void);
void yyerror(char *);

#define OPCODE_COUNT 256
#define ERRBUFLEN 1024

int linecounter = 1;
unsigned short address = 0x0000;
char error_msg[ERRBUFLEN];
struct identifier *identifiers = NULL;

static const char *mnemonics[OPCODE_COUNT] = {
/*         |  0    |  1    |  2    |  3    |  4    |  5    |  6    |  7     |  8    |  9    |  A    |  B  |    C  |    D  |    E    |  F    |      */
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

unsigned char opcode_lookup(char *mnemonic, char *mode) {
    for (unsigned char i = 0; i < OPCODE_COUNT-1; i++) {
        if (
            !strncmp(addressing_modes[i],   mode,       strlen(mode)) && 
            !strncmp(mnemonics[i],          mnemonic,   strlen(mnemonic))
        ) {
            return i;
        }        
    }

    snprintf(error_msg, ERRBUFLEN, "Opcode not found for mnemonic %s and addressing mode %s", mnemonic, mode);
    yyerror(error_msg);
    return 0x00;
}

/* implied addressing, this includes 's' and 'A' */
void implied(char *mnemonic) {
    unsigned char opcode = opcode_lookup(mnemonic, "i");

    printf("[%04x] i    %s %10s %02x\n", address, mnemonic, " ", opcode);

    address += 1;
    free(mnemonic);
}


/* direct addressing, can be zero page or absolute */
void direct(char *mnemonic, unsigned short operand) {
    if ((operand | 0x00ff) == 0x00ff) {
        printf("[%04x] zp   %s $%02x\n", address, mnemonic, operand);
        address += 2;
    } else {
        printf("[%04x] a    %s $%04x\n", address, mnemonic, operand);
        address += 3;
    }
    free(mnemonic);
}

void directive(char *directive, unsigned short addr) {
    if (strncmp("orig", directive, 4)) {
        address = addr;
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

void statement(char *mnemonic, char *operand, char *addressing_mode) {
    printf("%d mn: %s, op: %s, mode=%s\n", linecounter, mnemonic, operand, addressing_mode);
}

bool is_zp(char *operand) {
    // operand is zero page if:
    // - the operand looks like $xx, OR
    // - the operand is an identifier and its address is $xx, AND
    // - it is not in the identifiers list, because that it can only be a label (which is never zp)
    if (operand[0] == '$' && isxdigit(operand[1]) && isxdigit(operand[2]) && operand[3] == '\0') {
        return true;
    }

    unsigned short address = 0x0000;
    if (get_address(operand, &address)) {
        if ((address | 0x00ff) == 0x00ff) {
            return true;
        }
    }

    return false;
}

%}

%token MNEMONIC
%token ABSOLUTE
%token COMMAX
%token COMMAY
%token IDENTIFIER
%token DIRECTIVE
%token ZEROPAGE

%union {
    char *str;
}

%type<str> MNEMONIC
%type<str> DIRECTIVE
%type<str> IDENTIFIER
%type<str> ABSOLUTE
%type<str> ZEROPAGE
%type<str> zp_abs_identifier
%type<str> zp_abs
%type<str> zp_identifier

%%

program:
    program expression '\n' { linecounter++; }
|   program '\n'            { linecounter++; }
|	program error '\n'		{ yyerrok; }
|   /* NOTHING */
;

expression:
    MNEMONIC                                { statement($1, NULL,   "i"); }
|   MNEMONIC accu                           { statement($1, NULL,   "i"); }
|   MNEMONIC '#' zp_identifier              { statement($1, $3,     "#"); }
|   MNEMONIC zp_abs_identifier COMMAX       { statement($1, $2,     is_zp($2)?"zp,x":"a,x"); }
|   MNEMONIC zp_abs_identifier COMMAY       { statement($1, $2,     is_zp($2)?"zp,y":"a,y"); }
|   MNEMONIC zp_abs_identifier              { statement($1, $2,     is_zp($2)?"zp":"a"); }
|   MNEMONIC '(' zp_abs_identifier ')'      { statement($1, $3,     is_zp($3)?"(zp)":"(a)"); }
|   MNEMONIC '(' zp_identifier ')' COMMAY   { statement($1, $3,     "(zp),y"); }
|   MNEMONIC '(' zp_identifier COMMAX ')'   { statement($1, $3,     "(zp,x)"); }
|   IDENTIFIER '=' zp_abs                   { identifiers = register_identifier(identifiers, $1, strtol($3+1, NULL, 16)); }
|   DIRECTIVE ABSOLUTE                      { directive($1, strtol($2+1, NULL, 16)); }
;

accu:
    'A'
|   'a'
;

zp_identifier:
    ZEROPAGE
|   IDENTIFIER
;

zp_abs:
    ZEROPAGE
|   ABSOLUTE
;

zp_abs_identifier:
    ZEROPAGE
|   ABSOLUTE
|   IDENTIFIER
;

%%

