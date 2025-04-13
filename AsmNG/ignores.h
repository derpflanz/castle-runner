#ifndef __IGNORES_H
#define __IGNORES_H

#include <stdlib.h>
#include <stdbool.h>

char **ignores_create(char *s);
bool ignores_in_list(char **ignores, const char *s);
void ignores_free(char **ignores);
void ignores_print(char **arr);

#endif