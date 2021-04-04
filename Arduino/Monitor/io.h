#ifndef __IO_H
#define __IO_H

#include "Arduino.h"

#define R_NOTW  52
#define CE      53        // pin 37 = A15, which is used as chip enable

//const char ADDR[]  = { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45 };
const char ADDR[] = { 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 6, 7, 8, 9, 10, 11, 12, 13};
const char LEDS[] = { 22, 24, 26, 28, 30, 32, 34, 36 };
const char OUT2[] = { 46, 48, 50 };

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