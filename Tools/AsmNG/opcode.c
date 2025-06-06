#include "opcode.h"
#include <string.h>
#include <stdbool.h>

const char *mnemonics[OPCODE_COUNT] = {
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
const char *addressing_modes[OPCODE_COUNT] = {
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

// i = 0, r = 1, # = 1, a = 2, zp = 1
const int operand_length[OPCODE_COUNT] = {
/*        |  0  |  1       |  2    |  3  |  4     |  5     |  6     |  7     |  8  |  9    |  A  |  B  |  C   |    D   |  E   |  F  |        */
/* 0 */      0,    1,         0,      0,    1,       1,       1,       1,       0,    1,      0,    0,    2,       2,     2,     1,     /* 0 */
/* 1 */      1,    1,         1,      0,    1,       1,       1,       1,       0,    2,      0,    0,    2,       2,     2,     1,     /* 1 */
/* 2 */      2,    1,         0,      0,    1,       1,       1,       1,       0,    1,      0,    0,    2,       2,     2,     1,     /* 2 */
/* 3 */      1,    1,         1,      0,    1,       1,       1,       1,       0,    2,      0,    0,    2,       2,     2,     1,     /* 3 */
/* 4 */      0,    1,         0,      0,    0,       1,       1,       1,       0,    1,      0,    0,    2,       2,     2,     1,     /* 4 */
/* 5 */      1,    1,         1,      0,    0,       1,       1,       1,       0,    2,      0,    0,    0,       2,     2,     1,     /* 5 */
/* 6 */      0,    1,         0,      0,    1,       1,       1,       1,       0,    1,      0,    0,    2,       2,     2,     1,     /* 6 */
/* 7 */      1,    1,         1,      0,    1,       1,       1,       1,       0,    2,      0,    0,    2,       2,     2,     1,     /* 7 */
/* 8 */      1,    1,         0,      0,    1,       1,       1,       1,       0,    1,      0,    0,    2,       2,     2,     1,     /* 8 */
/* 9 */      1,    1,         1,      0,    1,       1,       1,       1,       0,    2,      0,    0,    2,       2,     2,     1,     /* 9 */
/* A */      1,    1,         1,      0,    1,       1,       1,       1,       0,    1,      0,    0,    2,       2,     2,     1,     /* A */
/* B */      1,    1,         1,      0,    1,       1,       1,       1,       0,    2,      0,    0,    2,       2,     2,     1,     /* B */
/* C */      1,    1,         0,      0,    1,       1,       1,       1,       0,    1,      0,    0,    2,       2,     2,     1,     /* C */
/* D */      1,    1,         1,      0,    0,       1,       1,       1,       0,    2,      0,    0,    0,       2,     2,     1,     /* D */
/* E */      1,    1,         0,      0,    1,       1,       1,       1,       0,    1,      0,    0,    2,       2,     2,     1,     /* E */
/* F */      1,    1,         1,      0,    0,       1,       1,       1,       0,    2,      0,    0,    0,       2,     2,     1      /* F */
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

int get_statement_length(const char *addressing_mode) {
    if (addressing_mode[0] == 'i') return 1;
    if (strchr(addressing_mode, 'a') != NULL) return 3;

    return 2;
}

int get_operand_length(const unsigned char opcode) {
    return operand_length[opcode];
}

unsigned short calculate_relative_address(const unsigned short from_address, const struct operand operand) {
    unsigned short target_address;

    if (!identifier_get(operand.str, &target_address)) {
        return 0x0000;
    }

    unsigned short result = target_address - from_address - 2;
    if (result < 0) {
        result += 256;
    }

    return result;
}

unsigned short calculate_operand(const struct operand operand) {
    unsigned short address = 0x0000;

    if (operand.str == NULL) {
        return address;
    }

    // 'X'
    if (strlen(operand.str) == 3 && operand.str[0] == '\'' && operand.str[2] == '\'') {
        return (unsigned short)operand.str[1];
    }

    // $90
    if (!identifier_get(operand.str, &address)) {
        address = strtol(operand.str+1, NULL, 16);
    }
    address += operand.offset;

    // >$90
    switch (operand.operation) {
        case '>':
            address = address >> 8;
        break;
        case '<':
            address = address & 0x00ff;
        break;
    }

    return address;
}