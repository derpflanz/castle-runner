#ifndef __UI_H
#define __UI_H

#include <ncurses.h>

void initui();
void update_ram_window(uint16_t base_address);
void writelog(const char *fmt, ...);

extern uint8_t *ram;

#endif