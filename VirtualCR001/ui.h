#ifndef __UI_H
#define __UI_H

#include <ncurses.h>

void ui_init();
void ui_init_lcd();
void ui_update_ram(uint16_t base_address);
void ui_writelog(const char *fmt, ...);

extern uint8_t *ram;

#endif