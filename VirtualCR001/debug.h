#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdint.h>

#define MAX_BREAKPOINTS 256

void debug_init(const char *debugfilename);

extern int breakpoint_hit;
extern uint16_t breakpoints[MAX_BREAKPOINTS];

#endif