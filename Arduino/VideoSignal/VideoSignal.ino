#include <Arduino.h>
#include <util/delay.h>

unsigned int scanline = 1;
unsigned int bar = 0;

ISR(TIMER1_OVF_vect)
{
    if (scanline == 200) {
        scanline = 1;
        PORTA &= ~0x02;
    }

    PORTA &= ~0x01;
    _delay_us(4.7);
    PORTA |= 0x01;

// ---- bars
    if (bar > 9) {
        PORTB ^= 0x02;
        bar = 0;
    }
    bar++;


// ----------

// ----- lines
    // if (scanline > 10) {
    //     PORTA |= 0x02;
    // } else {
    //     PORTA &= ~0x02;
    // }
    // scanline++;
    // if (scanline > 20) {
    //     scanline = 0;
    // }
// ------------


    //  PORTB ^= 0x02;
    //  _delay_us(5);
    //  PORTB ^= 0x02;
    // _delay_us(5);
    // PORTB ^= 0x02;
    // _delay_us(5);



    // reset timer to create a 64us horizontal scan
    TCNT1L = 0x64;
    TCNT1H = 0xFC;

    scanline++;
}

void setup() {
    DDRA |= 0xff;           // all output
    DDRB |= 0xff;
    PORTA = 0x00;
    PORTB = 0x00;

    OCR1AL = 1;
    OCR1AH = 0;
    TCCR1A = 0x00; //setting dispensing ratio
    TCCR1B = 0x01;      // clk = IO/1

    TIMSK1 = (1 << TOIE1);          // enable overflow interrupt
}
void loop() {
    //PORTA ^= 0xff;

    // PORTA |= 0x02;

    // _delay_us(64);

    // PORTA &= 0xFD;
}

