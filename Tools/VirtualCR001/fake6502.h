#ifndef __FAKE6502_H
#define __FAKE6502_H

#include <stdint.h>

void step6502();
void reset6502();
void exec6502(uint32_t tickcount);
void irq6502();

#define FLAG_CARRY     0x01
#define FLAG_ZERO      0x02
#define FLAG_INTERRUPT 0x04
#define FLAG_DECIMAL   0x08
#define FLAG_BREAK     0x10
#define FLAG_CONSTANT  0x20
#define FLAG_OVERFLOW  0x40
#define FLAG_SIGN      0x80

#endif