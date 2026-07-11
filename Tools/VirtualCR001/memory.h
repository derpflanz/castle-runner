#ifndef __MEMORY_H
#define __MEMORY_H

#include <ncurses.h>

#define _32K    32768
#define _64K    65536

#define V_QUIET  0
#define V_PRINTF 1

void mem_init();
int mem_readfile(const char *hexfilename, int verbose);
void mem_show(uint16_t base_address, uint16_t length);

uint8_t read6502(uint16_t address);
void write6502(uint16_t address, uint8_t value);

void set_io(int port, uint8_t value);

extern WINDOW *run;

#endif