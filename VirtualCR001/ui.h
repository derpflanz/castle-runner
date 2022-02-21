#ifndef __UI_H
#define __UI_H

#include <ncurses.h>

#define MEMLOG  1
#define IOLOG   2
#define WARNLOG 3

void ui_init();
void ui_init_lcd();
void ui_update_ram(uint16_t base_address);
void ui_writelog(int target, const char *fmt, ...);
void ui_print_lcd(char character, int row, int column);

extern uint8_t *ram;

// From the CPU
extern uint8_t sp, a, x, y, status;             // stack pointer, accu, x-reg, y-reg
extern uint16_t pc;                             // program counter
extern uint32_t instructions, clockticks6502;   // number of instructions run

#endif