#include "Arduino.h"
#include <stdarg.h>
#include "communication.h"

// this implements a blocking read
byte Comm::receiveByte() {
  byte ret = 0x00;
  while (!Serial.available());

  ret = Serial.read();
  return ret;
}

void Comm::sendByte(byte b) {
  Serial.write(b);
}

void Comm::setup() {
  Serial.begin(9600);
}

void Comm::printf(char *format, ...) {
  char linebuffer[80];
  va_list va;
  va_start(va, format);
  vsprintf(linebuffer, format, va);
  Serial.println(linebuffer);
  va_end(va);
}

Comm Communication;