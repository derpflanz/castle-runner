#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include "sound.h"

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

    init_freq_timer();
    init_duration_timer();
    load_song(song);
    set_speed(200);

    sei();

    while (1);
}
