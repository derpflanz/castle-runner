#ifndef __TREE_H
#define __TREE_H

#include "generated/parser.h"
#include <stdlib.h>

struct node {
    char *bytes;
    struct operand operand;

    struct node *next;
};

void tree_add_opcode(const char *mnemonic, struct operand operand, const char *addressing_mode);
void tree_add_string(const char *string);
void tree_add_byte(const char *byte);

extern struct node *tree;

#endif