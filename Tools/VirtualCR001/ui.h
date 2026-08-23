#ifndef __UI_H
#define __UI_H

#include <ncurses.h>

#define MEMLOG  1
#define IOLOG   2
#define WARNLOG 3

#define PORTB       0x4007
#define PORTA       0x4107
#define DDRB        0x4207
#define DDRA        0x4307
#define LatchCount  0x4407
#define Counter     0x4507
#define Latches     0x4607
#define Latch       0x4707
#define T2CL        0x4807
#define T2Counter   0x4907
#define Shift       0x4A07
#define Auxcontrol  0x4B07
#define Pericontrol 0x4C07
#define IRQFlag     0x4D07
#define IntENAB     0x4E07
#define PORTA_NOHS  0x4F07

void ui_init();
void ui_init_lcd();
void ui_update_ram(uint16_t base_address);
void ui_writelog(int target, const char *fmt, ...);
void ui_print_lcd(char character, int row, int column);
void ui_clear_log(int target);
void ui_set_ram(const char *name, uint16_t address);

extern uint8_t *ram;

// From the CPU
extern uint8_t sp, a, x, y, status;             // stack pointer, accu, x-reg, y-reg
extern uint16_t pc;                             // program counter
extern uint32_t instructions, clockticks6502;   // number of instructions run

#endif