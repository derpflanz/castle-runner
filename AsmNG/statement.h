#ifndef __STATEMENT_H
#define __STATEMENT_H

struct statement {
    unsigned char opcode;
    unsigned short operand;
    char *identifier;
};

#endif