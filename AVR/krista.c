#define F_CPU 16000000UL


#include <avr/io.h>
#include <util/delay.h>

void rgb(uint8_t red, uint8_t green, uint8_t blue) {
    OCR0A = red;
    OCR1A = green;
    OCR2A = blue;
}

int main() {
    DDRD |= (1 << DDD6);  // RED = OC0
    DDRB |= (1 << DDB1);  // GREEN = OC1
    DDRB |= (1 << DDB3);  // BLUE = OC2


    TCCR0A |= (1 << COM0A1);
    TCCR0A |=  (1 << WGM00) | (1 << WGM01);
    TCCR0B |= (1 << CS00);

    TCCR1A |= (1 << COM1A1);
    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << CS10);

    TCCR2A |= (1 << COM2A1);
    TCCR2A |= (1 << WGM20) | (1 << WGM21);
    TCCR2B |= (1 << CS20);

    // init compare
    OCR0A = 128;
    OCR1A = 128;
    OCR2A = 128;

    uint8_t green = 128;
    uint8_t red = 128;
    uint8_t blue = 128;

    uint8_t bl_dir = 1;
    uint8_t rd_dir = 1;
    uint8_t gr_dir = 1;

    while(1) {
        green += 3 * gr_dir;
        blue += 5 * bl_dir;
        red += 7 * rd_dir;

        if (green > 240 || green < 10) {
            gr_dir *= -1;
        }

        if (blue > 240 || blue < 10) {
            bl_dir *= -1;
        }

        if (red > 240 || red < 10) {
            rd_dir *= -1;
        }

        _delay_ms(30);
        rgb(red, green, blue);
    }
}