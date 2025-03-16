#include "io.h"

void init_io() {
    // Set R/!W, DR and CR as INPUTs (no pullup resistor)
    DDRD &= ~((1 << DDD2) | (1 << DDD3) | (1 << DDD4));
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

struct state read_state() {
    struct state io_state;

    io_state.rw = rw();
    io_state.dr = dr();
    io_state.cr = cr();

    return io_state;
}