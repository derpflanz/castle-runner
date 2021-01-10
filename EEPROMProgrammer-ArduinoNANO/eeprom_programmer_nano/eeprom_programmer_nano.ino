
#include "memory.h"
#include "communication.h"

void setup() {
  Communication.setup();
  Memory.setup();

  Communication.printf("EEPROM Programmer v2");
}

//void verify() {
//  for (int i = 0; i < 0x22; i++) {
//    unsigned char result = Read(i);
//
//    sprintf(linebuffer, "Read %u: (DEC %d; HEX: %x)", i, result, result);
//    PRINTLINE;
//  }
//}


void idle() {
  
}

byte receive_header() {
  return NOTHING;
}

void receive_data() {
  
}

void send_data() {
  
}

void loop() {
  idle();
  byte command = receive_header();  

  if (command == RECEIVE_DATA) {
    receive_data();
  } else if (command == SEND_DATA) {
    send_data();
  }
}

//// the loop function runs over and over again forever
//void __loop() {
//  if (Serial.available()) {
//    unsigned int size = 0;
//    unsigned int counter = 0;
//    
//    int b = Serial.read();
//    if (b == STX) {
//      while ( (b = Serial.read()) != ETX) {
//        if (b != -1) {
//          size = (size * 10) + (b - 0x30);
//        }
//      }
//    }
//
//    if (size > HEXFILE_MAX_SIZE) {
//      sprintf(linebuffer, "HEX file too large! Received %u, Max size is %u", size, HEXFILE_MAX_SIZE);
//      PRINTLINE;
//      Serial.write(NAK);
//    } else {
//      sprintf(linebuffer, "Going to read %u bytes from serial.", size);
//      PRINTLINE;
//      Serial.write(ACK);
//
//      while (counter < size) {
//        b = Serial.read();
//        if (b != -1) {
//          Write(counter, b);          
//          counter++;
//        }
//      }
//
//      b = Serial.read();
//      if (b == EOT) {
//        sprintf(linebuffer, "HEX reading successful.");
//        PRINTLINE;
//
//        // this ends the transmission
//        verify();
//        Serial.write(EOT);
//      }
//    }
//  }
//}
