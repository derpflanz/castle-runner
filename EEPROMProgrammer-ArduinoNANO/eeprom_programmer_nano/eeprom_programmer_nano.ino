#include "memory.h"
#include "communication.h"

void setup() {
  Communication.setup();
  Memory.setup();

  Memory.writeByte(0x03, 0xBF);

  Communication.printf("EEPROM Programmer v2");
  Communication.sendByte(SYN);
}

void idle() {
  byte recv;
  do {
    recv = Communication.readByte();
  } while (recv != SOH);
}

byte receive_header(int *data_length) {
  byte digit, command;
  *data_length = 0;
  command = Communication.readByte();

  while ( (digit = Communication.readByte()) != ETB) {
    *data_length = (*data_length * 10) + (digit - '0');
  }

  return command;
}

void receive_data() {
  
}

void send_data(int data_length) {
  Communication.sendByte(STX);

  for (int address = 0; address < data_length; address++) {
    byte data = Memory.readByte(address);
    Communication.sendByte(data);
  }

  Communication.sendByte(ETX);
}

void loop() {
  int data_length = 0;
  idle();
  byte command = receive_header(&data_length);

  if (command == WRITE_EEPROM) {
    receive_data();
  } else if (command == READ_EEPROM) {
    send_data(data_length);
  }

  Communication.sendByte(EOT);
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
