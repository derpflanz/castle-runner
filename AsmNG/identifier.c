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
    struct identifier *new_element = malloc(sizeof(struct identifier));
    new_element->name = strdup(ident);
    new_element->address = addr;
    new_element->next = identifiers;
    identifiers = new_element;

    free(ident);
}

bool identifier_get(char *ident, unsigned short *address) {
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

const char *identifier_reverse_lookup(unsigned short address) {
    struct identifier *p = identifiers;
    while (p) {
        if (p->address == address) {
            return p->name;
        }

        p = p->next;
    }

    return "";
}
