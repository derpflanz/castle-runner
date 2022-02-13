#include "log.h"
#include "io.h"
#include "clock.h"

void setup() {
  Io.setup();
  Log.setup();
  Clock.setup();
 // Clock.onTick(tick);

  Log.println("CR001 Control Board");
  Log.println("This is CR001 Monitor and Controller");
  Log.println("(c) 2022 BF");
}


void loop() {
  Clock.loop();
}

void tick(int speed) {
  Log.printf("Tick from main, speed: %d\r\n", speed);
  Io.read();

  Log.printf("Data: %02x; Address: %02x %02x\r\n", Io.data(), Io.addr_hi(), Io.addr_lo());
}