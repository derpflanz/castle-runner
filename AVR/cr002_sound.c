#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

uint8_t c = 0;
uint16_t c_f = 0;
uint8_t dir = 2;

uint8_t duration = 0;

uint16_t freq = 231;
int end = 0;
uint8_t note = 0;
uint8_t amplitude;

uint16_t attack_step;
uint16_t decay_step;
uint16_t sustain_step;
uint16_t release_step;


uint16_t notes_oct4[] = {
//  C    C#   D    D#   E    F    F#   G    G#   A    A#   B
    138, 146, 154, 164, 173, 184, 194, 206, 218, 231, 245, 260
};

#define O4_G    206
#define O4_A    231
#define O4_C    138
#define O4_B    260
#define O4_D    154
#define O4_F    184
#define O4_E    173
#define REST    0
#define END     65535

struct pair {
    uint16_t note;
    uint16_t attack;
    uint16_t decay;
    uint16_t sustain;
    uint16_t release;
};

struct pair current_note;

struct pair song[] = {
    { O4_C, 10, 1, 12, 1 },
    { REST, 1, 1, 0 , 1 },
    { O4_C, 1, 1, 8 , 1 },
    { REST, 1, 1, 2 , 1 },
    { O4_D, 1, 1, 8 , 1 },
    { REST, 1, 1, 2 , 1 },
    { O4_C, 1, 1, 8 , 1 },
    { REST, 1, 1, 2 , 1 },
    { O4_F, 1, 1, 8 , 1 },
    { REST, 1, 1, 2 , 1 },
    { O4_E, 1, 1, 16, 1 },
    { END,  1, 1, 0 , 1 }
};

char _sine[] = {
    125, 128, 131, 134, 137, 140, 143, 146, 149, 152, 156, 159, 162, 165, 168, 171, 174, 176, 179, 182, 185, 188, 191, 193, 196, 199, 201, 204, 206, 209, 211, 213, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 237, 239, 240, 242, 243, 245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 252, 251, 250, 249, 248, 247, 246, 245, 243, 242, 240, 239, 237, 236, 234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 179, 176, 174, 171, 168, 165, 162, 159, 156, 152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 99, 96, 93, 90, 87, 84, 81, 79, 76, 73, 70, 67, 64, 62, 59, 56, 54, 51, 49, 46, 44, 42, 39, 37, 35, 33, 31, 29, 27, 25, 23, 21, 19, 18, 16, 15, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 16, 18, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 42, 44, 46, 49, 51, 54, 56, 59, 62, 64, 67, 70, 73, 76, 79, 81, 84, 87, 90, 93, 96, 99, 103, 106, 109, 112, 115, 118, 121, 124
};

uint8_t sine() {
    c += 2;
    return _sine[c];
}

uint8_t sawtooth(uint8_t s) {
    c += s;
    return c;
}

uint8_t sawtooth_f(uint16_t s) {
    c_f += s;

    if (c_f > 32768) {
        c_f = 0;
    }

    return (uint8_t) (c_f / 128);
}

uint8_t low_saw() {
    static uint8_t subctr = 0;

    subctr++;

    if (subctr == 4) {
        subctr = 0;
        c++;
    }

    return c;
}

uint8_t triangle() {
    if (c == 252) {
        dir = -4;
    } else if (c == 0) {
        dir = 4;
    }
    
    c += dir;
    return c;
}

uint8_t noise() {
    uint8_t r = 0;

    r += rand();

    return r;
}


ISR(TIMER0_OVF_vect) {
    cli();
    uint8_t n = 0;

    // if (!(PINC & (1 << PC5))) {
    //     n = sawtooth_f(462);
    // }
    
    // if (!(PINC & (1 << PC4))) {
    //     n = sawtooth_f(231);
    //     n = _sine[n];
    // }

    if (duration > 0) {
        uint8_t waveform = sawtooth_f(freq);
        // waveform = _sine[waveform];

        uint16_t n_large = waveform * amplitude;
        
        n = n_large / 256;
    }

    OCR0A = n;

    sei();
}

ISR(TIMER1_COMPA_vect) {
    if (end == 1) return;

    cli();

    if (duration != 0) {
        if (duration < current_note.attack) {
            // we are in attack phase
            amplitude += attack_step;
        }

        if (duration > current_note.attack && 
            duration < current_note.attack + current_note.decay) {
            // we are in decay phase
            amplitude -= decay_step;
        }

        if (duration > current_note.attack + current_note.decay && 
            duration < current_note.attack + current_note.decay + current_note.sustain) {
            // we are in sustain phase
            amplitude += sustain_step;
        }

        if (duration > current_note.attack + current_note.decay + current_note.sustain) {
            amplitude -= release_step;
        }

        duration--;
    }

    if (duration == 0) {
        current_note = song[note];
        if (current_note.note == END) {
            freq = 0;
            end = 1;
        }
        
        // init duration
        duration = current_note.attack + current_note.decay + current_note.sustain + current_note.release;
        amplitude = 0;

        attack_step = 256 / current_note.attack;
        decay_step = 128 / current_note.decay;
        sustain_step = 0;
        release_step = 128 / current_note.release;

        freq = current_note.note;
        
        TCNT1 = 0;
        note++;
    }

    sei();
}

void init_freq_timer() {
    DDRD |= (1 << DDD6);        // OC0A as output

    // timer 0 parameters
    // COM0A1 = Clear OC0A on compare match, set at BOTTOM (=0)
    // This controls the *output* pin, not the timer itself    
    TCCR0A |= (1 << COM0A1);
    // Fast PWM mode; Runs always from 0x00 to 0xFF
    // TOV is always set at TOP (=0xFF), meaning TIMER0_OVF_vect is called
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    // Clock Select; CS2:0 = 001 -> No prescaling: runs at 16MHz
    TCCR0B |= (1 << CS00);

    // Enable the timer 0 overflow interupr (causes TIMER0_OVF_vext) to actually be called
    TIMSK0 |= (1 << TOIE0);

    // Init timer
    OCR0A = 0;
}

void init_duration_timer() {
    DDRB |= (1 << PB1);  // Set PB1 (OC1A) as output

    TCCR1B |= (1 << WGM12);  // CTC mode (Clear Timer on Compare Match)
    TCCR1A |= (1 << COM1A0); // Toggle OC1A on compare match

    OCR1A = 1952;  // 125ms at 16MHz with prescaler 1024

    TCCR1B |= (1 << CS12) ; //| (1 << CS10); // Prescaler 1024, start timer

    TIMSK1 |= (1 << OCIE1A);
}

int main() {
    cli();

    DDRC &= ~(1 << PC5);
//    PORTC |= (1 << PC5);
    DDRC |= (1 << PC0);
    

    init_freq_timer();
    init_duration_timer();

    sei();

    int prev_state = 0;
    // tone_on = 4;
    // TCNT1 = 0;
    // freq = 231;             // A4
    //duration = 2;

    while (1) {
//        uint8_t button_state = (PINC & (1 << PC5));

    }

    if (0) {
        if (duration) {
            PORTC |= (1 << PC0);         // Switch ON LED
        } else {
            PORTC &= ~(1 << PC0);
        }

        uint8_t button_state = (PINC & (1 << PC5));
        if (button_state != prev_state) {
            prev_state = button_state;

            if (button_state == 0) {
                if (note == 0) {
                    duration = 2;
                }
                if (note == 1) {
                    duration = 4;
                    freq = 462;
                }

                TCNT1 = 0;
                note++;
            }
        }
    }
}
