#include "log.h"
#include "io.h"

#define CLOCK   2
#define DIFF    7   // when high, show any change on the bus

byte data_shadow = 0;
byte addr_hi_shadow = 0;
byte addr_lo_shadow = 0;

void setup() {
  Io.setup();
  Log.setup();

  Log.println("Monitor Tool, for Arduino MEGA");
  Log.println("This is a 24-bit data monitor.");
  Log.println("(c) 2021 BF");

  pinMode(DIFF, INPUT);
  pinMode(CLOCK, INPUT);

  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
}

static void onClock() {
  Io.read();

  byte data = Io.data();
  byte addr_hi = Io.addr_hi();
  byte addr_lo = Io.addr_lo();

  printInfoLine(data, addr_hi, addr_lo);
}

void loop() {
  if (digitalRead(DIFF) == HIGH) {
    Io.read();

    byte data = Io.data();
    byte addr_hi = Io.addr_hi();
    byte addr_lo = Io.addr_lo();

    if (data != data_shadow || addr_lo != addr_lo_shadow || addr_hi != addr_hi_shadow) {
      printInfoLine(data, addr_hi, addr_lo);

      data_shadow = data;
      addr_hi_shadow = addr_hi;
      addr_lo_shadow = addr_lo;
    }
  }
}


static void printInfoLine(byte data, byte addr_hi, byte addr_lo) {
  int address = addr_hi << 8 |  addr_lo;

  Log.printf("CE = %d %s ", Io.chip_enable(), Io.chip_enable()==LOW?"RAM":"ROM");
  Log.printf("R/!W = %d %s ", Io.read_write(), Io.read_write()==HIGH?"READ ":"WRITE");

  Log.printf("address:   %02x %02x   ", addr_hi, addr_lo);

  Log.printBinary(addr_hi);
  Log.printf(" ");
  Log.printBinary(addr_lo);

  Log.printf("   data:   %02x   ", data);
  Log.printBinary(data);
  Log.printf(" ");

  if ((addr_hi & 0xf0) == 0x80) {
    Log.printOpcode(data);
  } else {
    Log.printAscii(data);
  }

  if (address == 0x7FF8) {
    Io.out1(data);
    Log.printf(" I/O 1: %02x", data);
  }

  if (address == 0x7FF9) {
    Io.out2(data);
    Log.printf(" I/O 2: %02x", data);
  }

  Log.flush();
}

