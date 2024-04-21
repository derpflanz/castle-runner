#include "identifier.h"
#include <string.h>
#include <stdio.h>

struct identifier *identifiers = NULL;

void identifier_print() {
    printf("Current list:\n");

    struct identifier *p = identifiers;

    while (p) {
        printf("element: name=%s, addr=%04x\n", p->name, p->address);
        p = p->next;
    }
}

void identifier_add(char *ident, unsigned short addr) {
    identifiers = register_identifier(identifiers, ident, addr);
}

struct identifier *register_identifier(struct identifier *list, char *ident, unsigned short addr) {
    struct identifier *new_element = malloc(sizeof(struct identifier));
    new_element->name = strdup(ident);
    new_element->address = addr;
    new_element->next = list;

    free(ident);

    return new_element;
}

bool get_address(char *ident, unsigned short *address) {
    struct identifier *p = identifiers;
    fprintf(stderr, "should be null: %p\n", identifiers);

    while (p) {
        if (!strncmp(ident, p->name, strlen(ident))) {
            *address = p->address;
            return true;
        }
        p = p->next;
    }

    return false;
}


