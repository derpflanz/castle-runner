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

    // EICRA = External Interrupt Control Register A
    // ISC0n = Interrupt Sense Control 0 Bit n
    // ISC0 = (1,0) => The falling edge of INT0 generates an interrupt request
    EICRA |= (1 << ISC01);          // SET   ISC01
    EICRA &= ~(1 << ISC00);         // CLEAR ISC00

    // ISC1n = Interrupt Sense Control 1 Bit n
    // ISC1 = (1,0) => The falling edge of INT1 generates an interrupt request
    EICRA |= (1 << ISC11);          // SET   ISC11
    EICRA &= ~(1 << ISC10);         // CLEAR ISC10

    // EIMSK = External Interrupt Mask Register
    // INT0 = Enable INT0
    // INT1 = Enable INT1
    EIMSK |= (1 << INT0);           // SET   INT0
    EIMSK |= (1 << INT1);           // SET   INT1
}

uint8_t data() {
    uint8_t d = 0;

    // read PC0-5
    d |=  (PINC & 0b00111111);
    // read PD0-1
    d |= ((PIND & 0b00000011) << 6);

    return d;
}
