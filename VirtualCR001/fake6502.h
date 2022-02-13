#ifndef __FAKE6502_H
#define __FAKE6502_H

#include <stdint.h>

void step6502();
void reset6502();
void exec6502(uint32_t tickcount);

#endif