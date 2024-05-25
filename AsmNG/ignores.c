#include "ignores.h"

#include <stdio.h>
#include <string.h>

void ignores_print(char **arr) {
    char **ptr = arr;

    while (*ptr) {
        printf("%s\n", *ptr);
        ptr++;
    }
}

char **ignores_create(char *s) {
    int elements = 1;
    char *ptr = s;
    while (*ptr) {
        if (*ptr == ',') elements++;
        ptr++;
    }

    printf("Found %d elements in %s\n", elements, s);

    char **result = malloc((elements + 1) * sizeof(char *));
    int i = 0;

    result[i] = s;

    ptr = s;
    while (*ptr) {
        if (*ptr == ',') {
            *ptr = '\0';
            result[++i] = ptr+1;
            ptr++;
        }
        ptr++;
    }

    result[i+1] = NULL;

    ignores_print(result);

    return result;
}

bool ignores_in_list(char **ignores, const char *s) {
    if (ignores == NULL || s == NULL) {
        return false;
    }

    char **ptr = ignores;

    while (*ptr) {
        if (!strcmp(s, *ptr)){
            return true;
        }
        ptr++;
    }

    return false;
}

void ignores_free(char **ignores) {

}
