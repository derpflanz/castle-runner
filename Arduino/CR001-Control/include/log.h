#ifndef __LOG_H
#define __LOG_H

#include "Arduino.h"

class Logger {
    public:
        void setup();

        void println(const char *s);
        void printf(const char *format, ...);

        void printBinary(byte b);
        void printAscii(byte b);

        void flush();
};

extern Logger Log;

#endif
