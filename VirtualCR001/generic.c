#include "generic.h"
#include <stdlib.h>
#include "ui.h"

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

void array_print(uint16_t *arr) {
    if (arr == NULL) return;
    int counter = 0;

    while (arr[counter] > 0) {
        ui_writelog(MEMLOG, "%04x\n", arr[counter++]);
    }
}