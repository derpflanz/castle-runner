#ifndef __MEMORY_H
#define __MEMORY_H

#include <ncurses.h>

#define _32K    32768
#define _64K    65536

void initialisememory();
int readfile(const char *hexfilename);
void showmem(uint16_t base_address, uint16_t length);
void _wshowmem(WINDOW *win, uint8_t *mem, uint16_t base_address);

extern WINDOW *run;

#endif