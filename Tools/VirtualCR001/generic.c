#include "generic.h"
#include <stdlib.h>

int array_contains(uint16_t needle, uint16_t *haystack) {
    if (haystack == NULL) return 0;
    int counter = 0;

    while (haystack[counter] > 0) {
        if (needle == haystack[counter++]) {
            return 1;
        }
    }

    return 0;
}