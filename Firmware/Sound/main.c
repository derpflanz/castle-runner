#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include <stdlib.h>
#include "sound.h"
#include "io.h"

// external registers
#define REG_CTRL        0
#define REG_FREQ_8BIT   1
#define REG_ATTK_8BIT   2
#define REG_DCAY_8BIT   3
#define REG_SUST_8BIT   4
#define REG_RLSE_8BIT   5

#define REGTYPE_8BIT    0
#define REGTYPE_16BIT   1

// Control bitmasks
#define CTRL_PLAY       1
#define CTRL_SAWT       2
#define CTRL_SINE       4
#define CTRL_TRNG       8

struct note happy_birthday[] = {
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

struct note current_note[] = 
{
    { O4_C, 10, 10, 10, 10 },
    { END,  1, 1, 1, 1 }
};

// The chip works with a register selector and a data channel
// Interrupt 0 selects the register to use (0-255)
// Interrupt 1 reads the data and writes it into the selected register
uint16_t *current_register;
int reg_type = REGTYPE_8BIT;
uint16_t ctrl_reg;

ISR(INT0_vect) {
    ctrl_reg = 0;
    current_register = NULL;
        
    // Select register (0-255)
    uint8_t reg = data();
    if ((reg & 0xf0) == 0) {
        reg_type = REGTYPE_8BIT;
    }

    switch (reg) {
        case REG_CTRL:
            current_register = &ctrl_reg;
        break;
        case REG_FREQ_8BIT:
            current_register = &(current_note[0].frequency);
            break;
        case REG_ATTK_8BIT:
            current_register = &(current_note[0].attack);
        break;
        case REG_RLSE_8BIT:
            current_register = &(current_note[0].release);
        break;
        case REG_SUST_8BIT:
            current_register = &(current_note[0].sustain);
        break;
        case REG_DCAY_8BIT:
            current_register = &(current_note[0].decay);
        break;
        default:
        break;
    }
}

// Write data into selected register
ISR(INT1_vect) {
    if (current_register == NULL) return;

    uint8_t d = data();
    *current_register = d;

    if (ctrl_reg != 0) {
        waveform_voice1 = NULL;            // sawtooth is default
        if (ctrl_reg & CTRL_TRNG) {
            waveform_voice1 = triangle;
        }
        if (ctrl_reg & CTRL_SINE)  {
            waveform_voice1 = sine;
        }
        if (ctrl_reg & CTRL_PLAY) {
            start_song();
        }
    }
}

int main() {
    cli();

    // initialise sound system
    init_freq_timer_voice_1();
    init_freq_timer_voice_2();
    init_duration_timer();
    load_song(happy_birthday);
    set_speed(500);
    waveform_voice1 = sine;
    init_io();

    sei();
    
    start_song();

    // everything is interrupt based, so no futher code needed
    while (1);
}
