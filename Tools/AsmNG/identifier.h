#ifndef __IDENTIFIER_H
#define __IDENTIFIER_H

#include <stdlib.h>
#include <stdbool.h>

struct identifier {
    char *name;
    unsigned short address;
    struct identifier *next;
};

void identifier_add(char *ident, unsigned short addr);
bool identifier_get(char *ident, unsigned short *address);
const char *identifier_reverse_lookup(unsigned short address);
void identifier_print();

#endif