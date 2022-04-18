#include "debug.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

//debugging helpers
int breakpoint_hit = 0;

void debug_init(const char *debug_filename) {
    FILE *debug_file = fopen(debug_filename, "r");

    if (debug_file == NULL) {
        if (errno == ENOENT) {
            printf("No debug file %s found. No debugging loaded.\n", debug_filename);
            return;
        } else {
            printf("Error trying to open debug file %s: %s (errno=%d). Debugging not loaded.\n", 
                debug_filename, strerror(errno), errno);
            return;
        }
    }
}
