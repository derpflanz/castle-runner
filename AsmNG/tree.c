#include "tree.h"
#include "generated/parser.h"
#include <string.h>
#include <stdio.h>

struct node *opcode_tree = NULL;
struct node *last_element = NULL;

void tree_add_opcode(unsigned short address, const char *mnemonic, struct operand operand, const char *addressing_mode) {
   struct node *node = malloc(sizeof(struct node));

    node->bytes = strdup(mnemonic);
    node->address = address;
    node->operand.str = operand.str==NULL?NULL:strdup(operand.str);
    node->operand.offset = operand.offset;
    node->operand.operation = operand.operation;
    node->operand.addressing_mode = strdup(addressing_mode);
    node->type = t_opcode;

    tree_add_node(node);
}

void tree_add_string(unsigned short address, const char *string) {
    struct node *node = malloc(sizeof(struct node));

    node->address = address;
    node->bytes = strdup(string);
    node->operand.str = NULL;
    node->type = t_string;

    tree_add_node(node);
}

void tree_add_byte(unsigned short address, const char *byte) {
    struct node *node = malloc(sizeof(struct node));

    node->address = address;
    node->bytes = strdup(byte);
    node->operand.str = NULL;
    node->type = t_byte;

    tree_add_node(node);
}

void tree_add_node(struct node *node) {
    node->next = NULL;

    if (opcode_tree == NULL) {
        opcode_tree = node;
        last_element = node;
        return;
    }

    last_element->next = node;
    last_element = node;    
}

void tree_print_node(struct node *node) {
    printf("Node: bytes=%s, operand=%s, offset=%d, operation=%c\n", node->bytes, 
        node->operand.str, node->operand.offset, node->operand.operation);
}

void tree_print() {
    struct node *ptr = opcode_tree;

    if (ptr == NULL) {
        printf("Tree is empty.\n");
        return;
    }

    while (ptr != NULL) {
        tree_print_node(ptr);
        ptr = ptr->next;
    }
}

struct node *tree_head() {
    return opcode_tree;
}