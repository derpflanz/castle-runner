#include "io.h"
#include "Arduino.h"

void IO::setup() {
    for (int i = 0; i < 24; i++) {
        pinMode(INPUTS[i], INPUT);
    }

    pinMode(IOACTIVE, INPUT);
}

void IO::read() {
  // 24 bits = 6 nibbles = 3 bytes
  // 0000 0000 0000 0000 0000 0000
  //    X    X    X    X    X    X
  //         A         A         A

  // data[0] = address LOW
  // data[1] = address HIGH
  // data[2] = databus

  byte data[3] = {0, 0, 0};
  byte mask = 0x01;
  
  for (int i = 0; i < 24; i++) {
    int pinState = digitalRead(INPUTS[i]);
    if (pinState == HIGH) {
      data[2 - (i / 8)] = data[2 - (i / 8)] | mask;
    }

    if (mask == 0x80) {
      mask = 0x01;
    } else {
      mask <<= 1;
    }
  }
  
  _addr_hi = data[1];
  _addr_lo = data[2];
  _data = data[0];
}

IO Io;