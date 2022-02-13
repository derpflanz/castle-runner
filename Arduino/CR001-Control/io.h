#ifndef __IO_H
#define __IO_H

#include "Arduino.h"

// Address bus are here A0 = Digital IN 52, etc
// Data bus is D0 = Digital IN 23 etc
const char INPUTS[] = {   
                        52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22, 
                        23, 25, 27, 29, 31, 33, 35, 37
                    };


// IOACTIVE is an INPUT
#define IOACTIVE    3

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
};

extern IO Io;

#endif;