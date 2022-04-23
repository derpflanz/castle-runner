#include "debug.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int breakpoint_hit = 0;
int debug_enabled = 0;

// list of breakpoints, address 0 is used as "end of array"
uint16_t breakpoints[MAX_BREAKPOINTS];

void read_breakpoints(FILE *debug_file) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int breakpoint_ctr = 0;

    while ((read = getline(&line, &len, debug_file)) != -1) {
        uint16_t address = strtoul(line, NULL, 16);
        breakpoints[breakpoint_ctr++] = address;

        if (breakpoint_ctr == MAX_BREAKPOINTS) {
            breakpoints[MAX_BREAKPOINTS - 1] = 0;
            break;
        }
    }

    if (line) {
        free(line);
    }
}

FILE *open_debug_file(const char *debug_filename) {
    FILE *debug_file = fopen(debug_filename, "r");

    if (debug_file == NULL) {
        if (errno == ENOENT) {
            printf("No debug file %s found. No debugging loaded.\n", debug_filename);
        } else {
            printf("Error trying to open debug file %s: %s (errno=%d). Debugging not loaded.\n", 
                debug_filename, strerror(errno), errno);
        }
    } else {
        printf("Found debug file %s.\n", debug_filename);
    }

    return debug_file;
}

void init_breakpoints() {
    memset(breakpoints, 0, MAX_BREAKPOINTS * sizeof(uint16_t));
}

extern uint8_t *ram;
void debug_init(const char *debug_filename) {
    FILE *debug_file = open_debug_file(debug_filename);
    if (debug_file == NULL) {
        return;
    }

    init_breakpoints();
    read_breakpoints(debug_file);
    
    fclose(debug_file);
}
