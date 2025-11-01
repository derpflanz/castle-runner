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

ISR(INT0_vect) {
    start_song();
}

int main() {
    cli();

    //uint8_t prev_rw = 1;

    // initialise sound system
    init_freq_timer();
    init_duration_timer();
    load_song(song);
    set_speed(1000);

    init_io();

    sei();

    start_song();

    while (1)
      {
        //  _delay_ms(1000);
         PORTB = 0xFF;
        //  _delay_ms(1000);
         PORTB = 0x00;

        // register uint8_t p = 1;
        // register uint8_t c = 1;
        // do
        // {
        //     p = c;
        //     c = (PIND & (1 << PD3) ? 1 : 0);
        // } while (!(c ==0 && p == 1));
        
        // start_song();

        // if (io_state.rw == 0 && prev_rw == 1) {
        //     //start_song();
        //     if (io_state.dr == 1 && io_state.cr == 0) {
        //         start_song();
        //     }
        // }
        // prev_rw = io_state.rw;

        // if (io_state.rw == 0 && prev_rw == 1) {
        //     //start_song();

        //     // we edged down, the MCU is writing
        //     // check which register to load
        //     if (io_state.reg_select == REG_NONE) {
        //         continue;
        //     }

        //     if (io_state.reg_select == REG_DATA) {
        //         start_song();
        //         if (io_state.data == CMD_STARTSONG) {
        //         }
        //     }
        // }

    }
}
