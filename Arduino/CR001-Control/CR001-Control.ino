#include "log.h"
#include "io.h"
#include "clock.h"

void setup() {
  Io.setup();
  Log.setup();
  Clock.setup();
  Clock.onTick(tick);

  Log.println("CR001 Control Board");
  Log.println("This is CR001 Monitor and Controller");
  Log.println("(c) 2022 BF");
}


void loop() {
  Clock.loop();
}

void tick(int speed) {
//  Log.printf("Tick from main, speed: %d\r\n", speed);
  Io.read();

  if (Io.addr_hi() == 0x40) {
    Log.printf("I/O: addr = 0x%02x%02x, data = %02x\r\n", Io.addr_hi(), Io.addr_lo(), Io.data());
  }
}