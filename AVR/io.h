#ifndef __IO_H
#define __IO_H

#include <avr/io.h>

// direction bits for 
// R/!W             PD4
// Data reg (DR)    PD2
// Ctrl reg (CR)    PD3
#define DIR_RW  DDD4
#define DIR_DR  DDD2
#define DIR_CR  DDD3

struct state {
    uint8_t rw;
    uint8_t dr;
    uint8_t cr;
};

void init_io();
struct state read_state();

#endif
