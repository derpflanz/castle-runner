#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

uint16_t slice = 0;
uint16_t total_duration = 0;

uint16_t freq = 231;
int end = 0;
uint8_t note_counter = 0;
uint8_t amplitude;


uint16_t end_of_attack, end_of_decay, end_of_sustain, end_of_release;

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
#define END     65535

struct note {
    uint16_t frequency;
    uint16_t attack;
    uint16_t decay;
    uint16_t sustain;
    uint16_t release;
};

struct note current_note;

struct note song[] = {
    { O4_C, 20, 10, 10, 10},
     { REST, 0, 0, 10 , 0 },
     { O4_C, 20, 10, 10 , 0 },
     { REST, 0, 0, 10 , 0 },
     { O4_D, 10, 10, 80 , 10 },
     { REST, 0, 0, 10 , 0 },
     { O4_C, 10, 10, 80 , 10 },
     { REST, 0, 0, 10 , 0 },
     { O4_F, 10, 10, 80 , 10 },
     { REST, 0, 0, 10 , 0 },
     { O4_E, 10, 10, 106, 10 },
    { END,  1, 1, 0 , 1 }
};

// waveform values (0-255)
uint8_t sine[] = {
    125, 128, 131, 134, 137, 140, 143, 146, 149, 152, 156, 159, 162, 165, 168, 171, 
    174, 176, 179, 182, 185, 188, 191, 193, 196, 199, 201, 204, 206, 209, 211, 213, 
    216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 237, 239, 240, 242, 243, 
    245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 254, 254, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 252, 251, 250, 249, 248, 
    247, 246, 245, 243, 242, 240, 239, 237, 236, 234, 232, 230, 228, 226, 224, 222, 
    220, 218, 216, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 
    179, 176, 174, 171, 168, 165, 162, 159, 156, 152, 149, 146, 143, 140, 137, 134, 
    131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 99, 96, 93, 90, 87, 84, 81, 
    79, 76, 73, 70, 67, 64, 62, 59, 56, 54, 51, 49, 46, 44, 42, 39, 37, 35, 33, 31, 
    29, 27, 25, 23, 21, 19, 18, 16, 15, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 
    16, 18, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 42, 44, 46, 49, 51, 54, 56, 59, 
    62, 64, 67, 70, 73, 76, 79, 81, 84, 87, 90, 93, 96, 99, 103, 106, 109, 112, 115, 118, 
    121, 124
};
uint8_t triangle[] = {
    2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 
    44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 
    84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 
    120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, 150, 
    152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 
    184, 186, 188, 190, 192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 
    216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 
    248, 250, 252, 254, 254, 252, 250, 248, 246, 244, 242, 240, 238, 236, 234, 232, 
    230, 228, 226, 224, 222, 220, 218, 216, 214, 212, 210, 208, 206, 204, 202, 200, 
    198, 196, 194, 192, 190, 188, 186, 184, 182, 180, 178, 176, 174, 172, 170, 168, 
    166, 164, 162, 160, 158, 156, 154, 152, 150, 148, 146, 144, 142, 140, 138, 136, 
    134, 132, 130, 128, 126, 124, 122, 120, 118, 116, 114, 112, 110, 108, 106, 104, 
    102, 100, 98, 96, 94, 92, 90, 88, 86, 84, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 
    62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22, 
    20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0, 
};

// This sawtooth function is used to calculate a uint8_t value from the, 
// uint16_t frequency, giving us more fine grained frequency steps (0-32768)
// The return value can then be used in another waveform (e.g. sine[])
uint8_t sawtooth(uint16_t step) {
    static uint16_t wvalue = 0;
    wvalue += step;

    if (wvalue > 32768) {
        wvalue = 0;
    }

    return (uint8_t) (wvalue / 128);
}

ISR(TIMER0_OVF_vect) {
    if (end == 1) return;

    cli();
    uint8_t n = 0;

    if (slice > 0) {
        uint8_t waveform_idx = sawtooth(freq);
        n = sine[waveform_idx];

        uint16_t n_large = n * amplitude;
        n = n_large / 256;
    }

    OCR0A = n;
    sei();
}

ISR(TIMER1_COMPA_vect) {
    static uint16_t attack_step;
    static uint16_t decay_step;
    static uint16_t release_step;

    if (end == 1) return;

    cli();

    if (slice < end_of_release) {
        if (slice < end_of_attack) {
            // we are in attack phase
            amplitude += attack_step;
        }

        if (slice > end_of_attack && slice < end_of_decay) {
            // we are in decay phase
            amplitude -= decay_step;
        }

        // Sustain is just that, nothing changes.

        if (slice > end_of_sustain) {
            amplitude -= release_step;
        }

        slice++;
    } else {
        // end of note reached
        current_note = song[note_counter];
        end = current_note.frequency == END;

        // init slice and amplitude
        slice = 0;
        amplitude = 0;

        // calculate envelope steps
        attack_step = 255 / current_note.attack;
        decay_step = 127 / current_note.decay;
        release_step = 127 / current_note.release;

        end_of_attack = current_note.attack;
        end_of_decay = end_of_attack + current_note.decay;
        end_of_sustain = end_of_decay + current_note.sustain;
        end_of_release = end_of_sustain + current_note.release;

        freq = current_note.frequency;
        
        TCNT1 = 0;
        note_counter++;
    }

    sei();
}

void init_freq_timer() {
    // OC0A as output, this is our signal output
    // future work could introduce OC0B as a second voice
    DDRD |= (1 << DDD6);        

    // timer 0 parameters
    // COM0A1 = Clear OC0A on compare match, set at BOTTOM (=0)
    // This controls the *output* pin, not the timer itself    
    TCCR0A |= (1 << COM0A1);

    // Fast PWM mode; Runs always from 0x00 to 0xFF
    // TOV is always set at TOP (=0xFF), meaning TIMER0_OVF_vect is called
    TCCR0A |= (1 << WGM00) | (1 << WGM01);

    // Clock Select; CS2:0 = 001 -> No prescaling: runs at 16MHz
    TCCR0B |= (1 << CS00);

    // Enable the timer 0 overflow interupr (causes TIMER0_OVF_vect) to actually be called
    TIMSK0 |= (1 << TOIE0);

    // Init compare register
    OCR0A = 0;
}

void init_duration_timer() {
    // CTC mode (Clear Timer on Compare Match)
    TCCR1B |= (1 << WGM12);  

    // Prescaler 64, gives a speed range on OCR1A between 100 (hyperturbo) and 2000 (very slow)
    TCCR1B |= (1 << CS11)| (1 << CS10);

    // Output Compare A Match Interrupt Enable: calls TIMER1_COMPA_vect
    TIMSK1 |= (1 << OCIE1A);

    // Initialise with a normal speed
    OCR1A = 1000;
}

int main() {
    cli();

    init_freq_timer();
    init_duration_timer();

    sei();

    while (1);
}
