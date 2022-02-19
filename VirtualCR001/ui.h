#ifndef __UI_H
#define __UI_H

#include <ncurses.h>

void ui_init();
void ui_init_lcd();
void ui_update_ram(uint16_t base_address);
void ui_writelog(const char *fmt, ...);
void ui_print_lcd(char character, int row, int column);

extern uint8_t *ram;

// From the CPU
extern uint8_t sp;          // stack pointer

#endif