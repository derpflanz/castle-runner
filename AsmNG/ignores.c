#include "ignores.h"

#include <stdio.h>
#include <string.h>

void ignores_print(char **arr) {
    if (arr == NULL) return;

    while (*arr) {
        printf("%s\n", *arr);
        arr++;
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

    result[i++] = s;

    ptr = s;
    while (*ptr) {
        if (*ptr == ',') {
            *ptr = '\0';
            result[i++] = ptr+1;
        }
        ptr++;
    }

    result[i] = NULL;

    ignores_print(result);

    return result;
}

bool ignores_in_list(char **ignores, const char *s) {
    if (ignores == NULL || s == NULL) {
        return false;
    }
    
    while (*ignores) {
        if (!strcmp(s, *ignores)){
            return true;
        }
        ignores++;
    }

    return false;
}

void ignores_free(char **ignores) {

}
