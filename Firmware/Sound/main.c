#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include "sound.h"
#include "io.h"

struct note song[] = {
    { O4_C, 10, 10,  10, 10 },
    { REST,  1,  1,  10,  1 },
    { O4_C, 10, 10,  10, 10 },
    { REST,  1,  1,  10,  1 },
    { O4_D, 10, 10,  80, 10 },
    { REST,  1,  1,  10,  1 },
    { O4_C, 10, 10,  80, 10 },
    { REST,  1,  1,  10,  1 },
    { O4_F, 10, 10,  80, 10 },
    { REST,  1,  1,  10,  1 },
    { O4_E, 10, 10, 160, 10 },
    { END,   1,  1,   1,  1 }
};

// The chip works with a register selector and a data channel
// Interrupt 0 selects the register to use (0-255)
// Interrupt 1 reads the data and writes it into the selected register

ISR(INT0_vect) {
    // Select register (0-255)
    uint8_t reg = data();

    if (reg == 0) {
        set_speed(2000);
        start_song();
    }

    if (reg == 1) {
        set_speed(500);
        start_song();
    }
}

ISR(INT1_vect) {

}

int main() {
    cli();

    // initialise sound system
    init_freq_timer();
    init_duration_timer();
    load_song(song);
    set_speed(1000);
    init_io();

    sei();

    // everything is interrupt based, so no loop needed
    while (1);
}
