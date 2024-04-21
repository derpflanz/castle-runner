#ifndef __TREE_H
#define __TREE_H

#include "generated/parser.h"
#include <stdlib.h>

enum node_type { t_byte, t_string, t_opcode };

struct node {
    unsigned short address;
    char *bytes;
    struct operand operand;
    enum node_type type;

    struct node *next;
};

struct node *tree_head();

void tree_add_opcode(unsigned short address,  const char *mnemonic, struct operand operand, const char *addressing_mode);
void tree_add_string(unsigned short address, const char *string);
void tree_add_byte(unsigned short address, const char *byte);
void tree_print();

void tree_add_node(struct node *node);
void tree_print_node(struct node *node);

#endif