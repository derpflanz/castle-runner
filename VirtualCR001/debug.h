#ifndef __DEBUG_H
#define __DEBUG_H

#define MAX_BREAKPOINTS 256

void debug_init(const char *debugfilename);

extern int breakpoint_hit;

#endif