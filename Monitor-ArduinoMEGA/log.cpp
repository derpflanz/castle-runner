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
        printf("  %c", b);
    } else {
        printf("---");
    }
}

void Logger::printOpcode(byte b) {
    switch (b) {
        case 0x8d:
        case 0x85:
            printf("STA");
            break;
        case 0xea:
            printf("NOP");
            break;
        case 0xa9:
        case 0xa5:
            printf("LDA");
            break;
        case 0x1a:
        case 0xe6:
            printf("INC");
            break;
        case 0x4c:
            printf("JMP");
            break;
        case 0x3a:
            printf("DEC");
            break;
        case 0x20:
            printf("JSR");
            break;
        case 0x60:
            printf("RTS");
            break;
        default:
            printf("%02x", b);
            break;
    }
}

void Logger::flush() {
    Serial.println();
    Serial.flush();
}

Logger Log;

