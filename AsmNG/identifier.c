#include "identifier.h"
#include <string.h>
#include <stdio.h>

void print_list(struct identifier *list) {
    printf("Current list:\n");

    struct identifier *p = list;

    while (p) {
        printf("element: name=%s, addr=%04x\n", p->name, p->address);
        p = p->next;
    }
}

struct identifier *register_identifier(struct identifier *list, char *ident, unsigned short addr) {
    struct identifier *new_element = malloc(sizeof(struct identifier));
    new_element->name = strdup(ident);
    new_element->address = addr;
    new_element->next = list;

    free(ident);

    return new_element;
}

struct identifier *identifiers = NULL;
