#ifndef __MEMORY_H
#define __MEMORY_H

#include <ncurses.h>

#define _32K    32768
#define _64K    65536

void mem_init();
int mem_readfile(const char *hexfilename);
void mem_show(uint16_t base_address, uint16_t length);
void _mem_wshow(WINDOW *win, uint8_t *mem, uint16_t base_address);

extern WINDOW *run;

#endif