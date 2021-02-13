#include "io.h"
#include "Arduino.h"

void IO::setup() {
    for (int i = 0; i < 24; i++) {
        pinMode(ADDR[i], INPUT_PULLUP);
    }
    for (int i = 0; i < 8; i++) {
        pinMode(LEDS[i], OUTPUT);
    }
    for (int i = 0; i < 3; i++) {
        pinMode(OUT2[i], OUTPUT);
    }
    pinMode(R_NOTW, INPUT);
}

void IO::read() {
  // 24 bits = 6 nibbles = 3 bytes
  // 0000 0000 0000 0000 0000 0000
  //    X    X    X    X    X    X
  //         A         A         A

  // data[0] = databus
  // data[1] = address HIGH
  // data[2] = address LOW

  byte data[3] = {0, 0, 0};
  byte mask = 0x01;
  
  for (int i = 0; i < 24; i++) {
    int pinState = digitalRead(ADDR[i]);
    if (pinState == HIGH) {
      data[2 - (i / 8)] = data[2 - (i / 8)] | mask;
    }

    if (mask == 0x80) {
      mask = 0x01;
    } else {
      mask <<= 1;
    }
  }

  _data = data[0];
  _addr_hi = data[1];
  _addr_lo = data[2];
}

void IO::out1(byte pattern) {
  byte mask = 0x01;

  for (int i = 0; i < 8; i++) {
    if ((pattern & mask) > 0) {
      digitalWrite(LEDS[i], HIGH);
    } else {
      digitalWrite(LEDS[i], LOW);
    }
    mask <<= 1;
  }
}

void IO::out2(byte pattern) {
  if (pattern & 0x01) {
    // enable
    digitalWrite(OUT2[0], HIGH);
  } else {
    digitalWrite(OUT2[0], LOW);
  }

  if (pattern & 0x02) {
    // rw
    digitalWrite(OUT2[1], HIGH);
  } else {
    digitalWrite(OUT2[1], LOW);
  }

  if (pattern & 0x04) {
    // RS
    digitalWrite(OUT2[2], HIGH);
  } else {
    digitalWrite(OUT2[2], LOW);
  }
}

IO Io;