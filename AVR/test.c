#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

#define PHASE_ATK 0
#define PHASE_DCY 1
#define PHASE_END 255

#define WAVE_SIN 0
#define WAVE_SAW 1
#define WAVE_TRI 2

#define C_NOTE 7
#define D_NOTE 9
#define E_NOTE 11
#define F_NOTE 12
#define G_NOTE 14
#define A_NOTE 16
#define B_NOTE 18

#define PI 3.1415

char triangle[] = {
    2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254, 254, 252, 250, 248, 246, 244, 242, 240, 238, 236, 234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 214, 212, 210, 208, 206, 204, 202, 200, 198, 196, 194, 192, 190, 188, 186, 184, 182, 180, 178, 176, 174, 172, 170, 168, 166, 164, 162, 160, 158, 156, 154, 152, 150, 148, 146, 144, 142, 140, 138, 136, 134, 132, 130, 128, 126, 124, 122, 120, 118, 116, 114, 112, 110, 108, 106, 104, 102, 100, 98, 96, 94, 92, 90, 88, 86, 84, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0
};

char sawtooth[]  = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

char sine[] = {
    125, 128, 131, 134, 137, 140, 143, 146, 149, 152, 156, 159, 162, 165, 168, 171, 174, 176, 179, 182, 185, 188, 191, 193, 196, 199, 201, 204, 206, 209, 211, 213, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 237, 239, 240, 242, 243, 245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 252, 251, 250, 249, 248, 247, 246, 245, 243, 242, 240, 239, 237, 236, 234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 179, 176, 174, 171, 168, 165, 162, 159, 156, 152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 99, 96, 93, 90, 87, 84, 81, 79, 76, 73, 70, 67, 64, 62, 59, 56, 54, 51, 49, 46, 44, 42, 39, 37, 35, 33, 31, 29, 27, 25, 23, 21, 19, 18, 16, 15, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 16, 18, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 42, 44, 46, 49, 51, 54, 56, 59, 62, 64, 67, 70, 73, 76, 79, 81, 84, 87, 90, 93, 96, 99, 103, 106, 109, 112, 115, 118, 121, 124
};

// the atk/dcy parameters follow the C64 table as much as possible
// if envelope increases with 1, we have a 12.8ms ATK/DCY (256 * 50us)
// for <12.5ms we need to increase faster:
// "6" for 2ms (ATK=0): (255 / 6) * 50us = 2125us = 2.1ms
// "2" for 6ms (ATK=1): (255 / 2) * 50us = 6375us = 6.4ms (C64 table says 8ms)
int attack[] = {
    // 2, 8             -- faster than 12.5ms
       6, 2, 
    // 16, 24, 38, 56, 68, 80, 100, 250, 500, 800, 1000, 3000, 5000, 8000 
       1,   2,  3,  4,  5,  6,   8,  20,  40,  64,   80,  240,  500,  640
};

int decay[] = {
    // 6,               -- faster than 12.5ms
       2,    
    // 24, 48, 72, 114, 168, 204, 240, 300, 750, 1500, 2400, 3000, 9000, 15000, 24000
        2,  4,  6,   9,  13,  16,  19,  24,  60,  120,  192,  240,  720,  1200,  1920
};

int get_attack_envelope(int atk, int envelope) {
    static int envelope_ctr = 0;

    if (atk < 2) {
        envelope += attack[atk];
    } else {
        if (envelope_ctr > attack[atk]) {
            envelope_ctr = 0;
            envelope++;
        }
        envelope_ctr++;
    }

    return envelope;
}

int get_decay_envelope(int dcy, int envelope) {
    static int envelope_ctr = 0;

    if (dcy < 2) {
        envelope -= decay[dcy];
    } else {
        if (envelope_ctr > decay[dcy]) {
            envelope_ctr = 0;
            envelope--;
        }
        envelope_ctr++;
    }

    return envelope;
}

void note(int freq, int atk, int dcy, int waveform) {    
    int phase = PHASE_ATK;
    uint8_t output = 0;
    uint16_t ampl_output;
    uint8_t t = 0;              // time
    int envelope = 0;

    while (phase < PHASE_END) {
        if (phase == PHASE_ATK) {
            envelope = get_attack_envelope(atk, envelope);
            if (envelope > 128) {
                phase = PHASE_DCY;
            }
        }

        if (phase == PHASE_DCY) {
            envelope = get_decay_envelope(dcy, envelope);
            if (envelope < 10) {
                phase = PHASE_END;
            }
        }

        if (waveform == WAVE_SIN) {
            output = sine[t];
        } else if (waveform == WAVE_SAW) {
            output = sawtooth[t];
        } else if (waveform == WAVE_TRI) {
            output = triangle[t];
        }

        ampl_output = envelope * output;
        PORTD = (uint8_t)(ampl_output >> 8);

        // time between samples is 50us, or 20kHz, so max freq of a sound signal is 10kHz
        _delay_us(100);
        // t is a uint8_t, we allow it to wrap around, our signal arrays are 256 items long
        t += freq;        
    }
}


// Ode To Joy
// E E F G
// G F E D
// C C D E E D C
// E E F G
// G F E D
// C C D E D C C

uint8_t c = 128;
uint8_t q = 0;
uint8_t dir = 1;
double x = 0;
double sin_step = 0.1; //(2.0*PI)/255.0;

uint8_t sawtooth(c) {    
    return c++;
}

uint8_t triangle(c) {
    if (c == 254) {
        dir = -2;
    }
    
    if (c == 0) {
        dir = 2;
    }
    
    c += dir;
    return c;
}

ISR(TIMER0_OVF_vect) {
    cli();

    OCR0A = sawtooth();

    sei();
}

int main() {
    DDRD |= (1 << DDD6);
    // PD6 is now an output

    DDRC |= (1 << DDC0);
    PORTC |= (1 << PORTC0);         // Switch ON LED

    TCCR0A |= (1 << COM0A1);       
    TCCR0A |=  (1 << WGM00) | (1 << WGM01);

//    TCCR0B |= (1 << WGM02);
    TCCR0B |= (1 << CS00);

    TIMSK0 |= (1 << TOIE0);

    // init compare
    OCR0A = c;

    sei();

    while (1);

        // while (0)
        // {

        
        //     OCR0A = c;
        //     // set PWM for 50% duty cycle

        //     //_delay_ms(1);
        //     _delay_us(6);

        //     c += direction;
        //     if (c == 0) {
        //         direction *= -1;
        //         c += direction;
        //     }

        //     // OCR0A = 5;

        //     // _delay_ms(1000);

        // }
}



int _main() {
    DDRC = 0x01;            // C0 = output (led), C1 = input
    PORTC |= (1 << PORTC1); // set internal pull up
    DDRD = 0xff;            // PORTD is our 8 bit audio output
    
    while (1) {
        PORTC |= (1 << PORTC0);

        _delay_ms(1);

        PORTC &= ~(1 << PORTC0);

        _delay_ms(1);
    }

}

int __main() {
    DDRC = 0x01;            // C0 = output (led), C1 = input
    PORTC |= (1 << PORTC1); // set internal pull up
    DDRD = 0xff;            // PORTD is our 8 bit audio output

    while(1) {
        int button_pressed = !(PINC & (1 << PORTC1));

        if (button_pressed == 1) {
            PORTC |= (1 << PORTC0);

            note(E_NOTE, 2, 4, WAVE_TRI);
            note(E_NOTE, 2, 4, WAVE_TRI);
            note(F_NOTE, 2, 4, WAVE_TRI);
            note(G_NOTE, 2, 4, WAVE_TRI);

            note(G_NOTE, 2, 4, WAVE_TRI);
            note(F_NOTE, 2, 4, WAVE_TRI);
            note(E_NOTE, 2, 4, WAVE_TRI);
            note(D_NOTE, 2, 4, WAVE_TRI);

            note(C_NOTE, 2, 4, WAVE_TRI);
            note(C_NOTE, 2, 4, WAVE_TRI);
            note(D_NOTE, 2, 4, WAVE_TRI);
            note(E_NOTE, 2, 4, WAVE_TRI);
            note(E_NOTE, 2, 8, WAVE_TRI);
            note(D_NOTE, 2, 2, WAVE_TRI);
            note(C_NOTE, 2, 2, WAVE_TRI);

            _delay_ms(20);

            note(E_NOTE, 2, 4, WAVE_SAW);
            note(E_NOTE, 2, 4, WAVE_SAW);
            note(F_NOTE, 2, 4, WAVE_SAW);
            note(G_NOTE, 2, 4, WAVE_SAW);

            note(G_NOTE, 2, 4, WAVE_SAW);
            note(F_NOTE, 2, 4, WAVE_SAW);
            note(E_NOTE, 2, 4, WAVE_SAW);
            note(D_NOTE, 2, 4, WAVE_SAW);

            note(C_NOTE, 2, 4, WAVE_SAW);
            note(C_NOTE, 2, 4, WAVE_SAW);
            note(D_NOTE, 2, 4, WAVE_SAW);
            note(E_NOTE, 2, 4, WAVE_SAW);
            note(D_NOTE, 2, 8, WAVE_SAW);
            note(C_NOTE, 2, 2, WAVE_SAW);
            note(C_NOTE, 2, 4, WAVE_SAW);

            PORTC &= ~(1 << PORTC0);
        }        
    }
}
