#ifndef __SOUND_H
#define __SOUND_H

#include <avr/io.h>

// The frequency number is a uint16_t and calculated as follows:
// n = f / (f_clk / 256 / 32768)
// n = f / 1.9073486328125
// Octave 4, A is 440Hz
#define O4_C    138
#define O4_CS   146
#define O4_D    154
#define O4_DS   164
#define O4_E    173
#define O4_F    184
#define O4_G    206
#define O4_A    231         // 440 Hz
#define O4_B    260

#define REST    0
#define BEGIN   65534
#define END     65535

struct note {
    uint16_t frequency;
    uint16_t attack;
    uint16_t decay;
    uint16_t sustain;
    uint16_t release;
};

void init_freq_timer();
void init_duration_timer();
uint8_t sawtooth(uint16_t frequency);
void load_song(struct note *song);
void set_speed(uint16_t speed);
void start_song();

extern uint8_t sine[];
extern uint8_t triangle[];

#endif