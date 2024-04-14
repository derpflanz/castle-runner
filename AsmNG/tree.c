#include "tree.h"
#include "generated/parser.h"
#include <string.h>
#include <stdio.h>

void tree_add_opcode(const char *mnemonic, struct operand operand, const char *addressing_mode) {

}

void tree_add_string(const char *string) {

}

void tree_add_byte(const char *byte) {
    struct node *node = malloc(sizeof(struct node));

    node->bytes = strdup(byte);
    node->operand.str = NULL;
}

void tree_print_node(struct node *node) {
    printf("Node: bytes=%s, operand=%s, offset=%d, operation=%c\n", node->bytes, 
        node->operand.str, node->operand.offset, node->operand.operation);
}

void tree_print(struct node *tree) {
    struct node *ptr = tree;

    if (ptr == NULL) {
        printf("Tree is empty.\n");
        return;
    }

    while (ptr != NULL) {
        tree_print_node(ptr);
        ptr = ptr->next;
    }
}

struct node *tree = NULL;