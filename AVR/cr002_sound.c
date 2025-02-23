#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t c = 0;
uint8_t dir = 2;

uint8_t sawtooth() {    
    return c++;
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

ISR(TIMER0_OVF_vect) {
    cli();

    OCR0A = triangle();

    sei();
}

int main() {
    DDRD |= (1 << DDD6);

    DDRC |= (1 << DDC0);
    PORTC |= (1 << PORTC0);         // Switch ON LED

    TCCR0A |= (1 << COM0A1);       
    TCCR0A |=  (1 << WGM00) | (1 << WGM01);

    TCCR0B |= (1 << CS00);

    TIMSK0 |= (1 << TOIE0);

    c = 0;
    OCR0A = c;

    sei();
    while (1);
}
