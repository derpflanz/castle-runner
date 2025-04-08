#include "io.h"

// PD4 = ~R/W
// PD5 = CTRL_REG
// PD6 = DATA_REG
// PC0-PC5,PD0-PD1 = DATA

void init_io() {
    DDRD &= ~(
        // Set R/!W, DR and CR as INPUTs (no pullup resistor)
        (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | 

        // Set D6 and D7 as INPUTs (no pullup)
        (1 << DDD0) | (1 << DDD1)
    );

    DDRC &= ~(
        // Set D0-5 as INPUTs (no pullup)
        (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | 
        (1 << DDC3) | (1 << DDC4) | (1 << DDC5) 
    )
}

uint8_t rw() {
    return (PIND & (1 << PD4) ? 1 : 0);
}

uint8_t dr() {
    return (PIND & (1 << PD2) ? 1 : 0);
}

uint8_t cr() {
    return (PIND & (1 << PD3) ? 1 : 0);
}

uint8_t data() {
    uint8_t d = 0;

    // read PC0-5
    d |= (PINC & 0b00111111);
    // read PD0-1
    d |= ((PIND & 0b00000011) << 6);

    return d;
}

struct state read_state() {
    struct state io_state;

    io_state.rw = rw();
    io_state.dr = dr();
    io_state.cr = cr();
    io_state.data = data();
    
    io_state.reg_select = REG_NONE;

    if (io_state.dr == 1 && io_state.cr == 0) {
        io_state.reg_select = REG_DATA;
    }

    if (io_state.dr == 0 && io_state.cr == 1) {
        io_state.reg_select = REG_CTRL;
    }

    return io_state;
}