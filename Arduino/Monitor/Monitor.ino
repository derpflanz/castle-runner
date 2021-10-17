#include "log.h"
#include "io.h"

#define CLOCK   3
#define DIFF    4   // when high, show any change on the bus
#define AUX     5

byte data_shadow = 0;
byte addr_hi_shadow = 0;
byte addr_lo_shadow = 0;
byte rw_shadow = 0;
byte aux_shadow = 0;
byte diff_shadow = 0;

bool clock_edge = false;
bool address_valid = false;
bool data_valid = false;
bool data_read = false;
bool address_read = false;

byte data = 0;
byte addr_hi = 0;
byte addr_lo = 0;
byte r_w = LOW;

void setup() {
  Io.setup();
  Log.setup();

  Log.println("Monitor Tool, for Arduino MEGA");
  Log.println("This is a 24-bit data monitor.");
  Log.println("(c) 2021 BF");

  pinMode(DIFF, INPUT);
  pinMode(CLOCK, INPUT);
  pinMode(AUX, INPUT);

  //attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, CHANGE);  
  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, FALLING);
}

static void onClock() {
  Io.read();

  data = Io.data();
  addr_hi = Io.addr_hi();
  addr_lo = Io.addr_lo();
  r_w = Io.read_write();

  clock_edge = true;
}

static void _onClock() {
  byte c = digitalRead(CLOCK);

  if (c == HIGH) {
    
  } else if (c == LOW) {
    data_valid = true;
    address_valid = true;
    r_w = Io.read_write();
  }
}

void doInfoLineOnClock() {
  Io.read();

  byte data = Io.data();
  byte addr_hi = Io.addr_hi();
  byte addr_lo = Io.addr_lo();

  printInfoLine("CLK ", data, addr_hi, addr_lo, digitalRead(AUX));
}

void loop() {
  if (data_valid == true) {
    Io.read();
    data = Io.data();

    data_valid = false;
    data_read = true;
  }

  if (address_valid == true) {
    Io.read();
    addr_hi = Io.addr_hi();
    addr_lo = Io.addr_lo();

    address_valid = false;
    address_read = true;
  }

  if (clock_edge == true) {
    printInfoLine("CLK ", data, addr_hi, addr_lo, r_w);

    clock_edge = false;
    //address_read = false;
    //data_read = false;
  }

  if (digitalRead(DIFF) == HIGH) {
    Io.read();

    byte data = Io.data();
    byte addr_hi = Io.addr_hi();
    byte addr_lo = Io.addr_lo();
    byte rw = Io.read_write();
    byte aux = digitalRead(AUX);
    
    if (data != data_shadow || addr_lo != addr_lo_shadow || addr_hi != addr_hi_shadow || aux != aux_shadow || rw != rw_shadow) {
      printInfoLine("DIFF", data, addr_hi, addr_lo, rw);

      data_shadow = data;
      addr_hi_shadow = addr_hi;
      addr_lo_shadow = addr_lo;
      aux_shadow = aux;
      rw_shadow = rw;
    }
  }
}

static void printInfoLine(const char *ident, byte data, byte addr_hi, byte addr_lo, byte rw) {
  int address = addr_hi << 8 |  addr_lo;

  Log.printf("%s CE = %d %s ", ident, Io.chip_enable(), Io.chip_enable()==LOW?"RAM":"ROM");
  Log.printf("R/!W = %d %s ", rw, rw==HIGH?"READ ":"WRITE");

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

//  Log.printf("   aux: %02x   ", aux);

  if (address >= 0x4000 && address <= 0x7fff) {
    Io.out1(data);
    Log.printf(" I/O %04x: %02x", address, data);
  }

  Log.flush();
}

