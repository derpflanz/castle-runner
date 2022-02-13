#include "log.h"
#include "Arduino.h"

void Logger::setup() {
    Serial.begin(115200);
}

void Logger::println(const char *s) {
    Serial.println(s);
}

void Logger::printf(const char *format, ...) {
  char linebuffer[80];
  va_list va;
  va_start(va, format);
  vsprintf(linebuffer, format, va);
  Serial.print(linebuffer);
  va_end(va);
}

void Logger::printBinary(byte b) {
  unsigned char mask = 0x80;
  for (char i = 0; i < 8; i++) {
    if (i == 4) printf(".");
    printf((b & mask)?"1":"0");
    mask >>= 1;
  }
}

void Logger::printAscii(byte b) {
    if (b >= 0x20 && b <= 0x7E) {
        printf("'%c'", b);
    } else {
        printf("---");
    }
}

void Logger::flush() {
    Serial.println();
    Serial.flush();
}

Logger Log;
