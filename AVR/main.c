#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include "sound.h"
#include "io.h"

struct note song[] = {
    { O4_C, 20, 10,  10, 10 },
    { REST,  1,  1,  10,  1 },
    { O4_C, 20, 10,  10,  1 },
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

int main() {
    cli();

    uint8_t prev_rw = 1;

    // initialise sound system
    init_freq_timer();
    init_duration_timer();
    load_song(song);
    set_speed(1000);

    init_io();

    sei();

    while (1) {
        struct state io_state = read_state();

        if (io_state.rw == 0 && prev_rw == 1) {
            // we edged down, the MCU is writing
            // check which register to load
            

            start_song();
        }

        prev_rw = io_state.rw;
    }
}
