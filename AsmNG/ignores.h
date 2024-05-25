#ifndef __IGNORES_H
#define __IGNORES_H

#include <stdlib.h>
#include <stdbool.h>

const char **ignores_create(const char *s);
bool ignores_in_list(const char **ignores, const char *s);

#endif