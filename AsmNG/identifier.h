#ifndef __IDENTIFIER_H
#define __IDENTIFIER_H

#include <stdlib.h>

struct identifier {
    char *name;
    unsigned short address;
    struct identifier *next;
};

struct identifier *register_identifier(struct identifier *list, char *ident, unsigned short addr);
void print_list(struct identifier *list);

#endif