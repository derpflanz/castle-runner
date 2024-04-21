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
struct identifier *register_identifier(struct identifier *list, char *ident, unsigned short addr);
void identifier_print();
bool get_address(char *ident, unsigned short *address);

#endif