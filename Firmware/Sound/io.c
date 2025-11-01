#include "io.h"
#include <avr/io.h>

// PD2 => INT0 => Address select (low active)
// PC0-PC5,PD0-PD1 = DATA

void init_io() {
    DDRD &= ~(
        // Set R/!W, DR and CR as INPUTs (no pullup resistor)
        (1 << DDD2) | //(1 << DDD3) | (1 << DDD4) | 

        // Set D6 and D7 as INPUTs (no pullup)
        (1 << DDD0) | (1 << DDD1)
    );

    DDRC &= ~(
        // Set D0-5 as INPUTs (no pullup)
        (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | 
        (1 << DDC3) | (1 << DDC4) | (1 << DDC5) 
    );

    // Configure INT0 (PD2) for a falling edge trigger
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);

    EIMSK |= (1 << INT0);
}

uint8_t data() {
    uint8_t d = 0;

    // read PC0-5
    d |=  (PINC & 0b00111111);
    // read PD0-1
    d |= ((PIND & 0b00000011) << 6);

    return d;
}
