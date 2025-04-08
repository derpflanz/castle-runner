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

#define REG_NONE    0
#define REG_DATA    1
#define REG_CTRL    2

struct state {
    uint8_t rw;
    uint8_t dr;
    uint8_t cr;
    uint8_t data;

    uint8_t reg_select;
};

void init_io();
struct state read_state();

#endif
