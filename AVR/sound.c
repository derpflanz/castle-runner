#include <avr/io.h>

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