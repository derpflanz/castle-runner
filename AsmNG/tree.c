#include "tree.h"
#include "generated/parser.h"
#include <string.h>
#include <stdio.h>

void tree_add_opcode(const char *mnemonic, struct operand operand, const char *addressing_mode) {
   struct node *node = malloc(sizeof(struct node));

    node->bytes = strdup(mnemonic);
    node->operand.str = operand.str==NULL?NULL:strdup(operand.str);
    node->operand.offset = operand.offset;
    node->operand.operation = operand.operation;

    tree_add_node(node);
}

void tree_add_string(const char *string) {
    struct node *node = malloc(sizeof(struct node));

    node->bytes = strdup(string);
    node->operand.str = NULL;

    tree_add_node(node);
}

void tree_add_byte(const char *byte) {
    struct node *node = malloc(sizeof(struct node));

    node->bytes = strdup(byte);
    node->operand.str = NULL;

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

struct node *opcode_tree = NULL;
struct node *last_element = NULL;