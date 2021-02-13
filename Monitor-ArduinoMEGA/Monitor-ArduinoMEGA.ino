#include "log.h"
#include "io.h"

#define CLOCK   2

void setup() {
  Io.setup();
  Log.setup();

  Log.println("Monitor Tool, for Arduino MEGA");
  Log.println("This is a 24-bit data monitor.");
  Log.println("(c) 2021 BF");

  pinMode(CLOCK, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
}

static void onClock() {
  Io.read();

  byte data = Io.data();
  byte addr_hi = Io.addr_hi();
  byte addr_lo = Io.addr_lo();

  int address = addr_hi << 8 |  addr_lo;

  Log.printf("CE = %d %s ", Io.chip_enable(), Io.chip_enable()==LOW?"RAM":"ROM");
  Log.printf("R/!W = %d %s ", Io.read_write(), Io.read_write()==HIGH?"READ ":"WRITE");

  Log.printf("address: %02x %02x ", addr_hi, addr_lo);

  Log.printBinary(addr_hi);
  Log.printf(" ");
  Log.printBinary(addr_lo);

  Log.printf("   data: ");
  Log.printBinary(data);
  Log.printf(" ");

  if ((addr_hi & 0xf0) == 0x80) {
    Log.printOpcode(data);
  } else {
    Log.printAscii(data);
  }

  if (address == 0x7FFA) {
    Io.out1(data);
    Log.printf(" I/O 1: %02x", data);
  }

  if (address == 0x7FFB) {
    Io.out2(data);
    Log.printf(" I/O 2: %02x", data);
  }

  Log.flush();
}

void loop() {
}
