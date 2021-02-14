#ifndef __IO_H
#define __IO_H

#include "Arduino.h"

#define R_NOTW  3
#define CE      37        // pin 37 = A15, which is used as chip enable

const char ADDR[]  = { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45 };
const char LEDS[] = { 4, 5, 6, 7, 8, 9, 10, 11 };
const char OUT2[] = { 46, 47, 48 };

typedef int pin;

class IO {
    private:
        byte _data;
        byte _addr_hi;
        byte _addr_lo;
    public:
        void setup();
        void read();

        byte data() { return _data; }
        byte addr_lo() { return _addr_lo; }
        byte addr_hi() { return _addr_hi; }

        pin chip_enable() { return digitalRead(CE); }
        pin read_write() { return digitalRead(R_NOTW); }

        void out1(byte pattern);
        void out2(byte pattern);
};

extern IO Io;

#endif;