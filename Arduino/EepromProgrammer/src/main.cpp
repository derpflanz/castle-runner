#include "memory.h"
#include "communication.h"
#include <Arduino.h>

byte resb_lo = 0;
byte resb_hi = 0;

struct header {
  uint16_t start_address;       // address where to start reading/writing
  uint16_t length;              // length of read/write
  uint16_t resb;                // value to store in the RESB vector (0xfffc-0xfffd)
  uint16_t irq;                 // value to store in the IRQ/BRK vector (0xfffe-0xffff)
  uint16_t nmi;                 // value to store in the NMI vector (0xfffa-0xfffb)
};

void setup() {
  Communication.setup();
  Memory.setup();
}

void idle() {
  byte recv;
  do {
    recv = Communication.receiveByte();
  } while (recv != SYN);

  Communication.printf("EEPROM Programmer v2");
  Communication.sendByte(SYN);
}

byte receive_header(header *the_header) {
  byte digit, recv, command = CMD_NOTHING;

  do {
    recv = Communication.receiveByte();
  } while (recv != SOH);

  the_header->length = 0;
  command = Communication.receiveByte();

  while ( (digit = Communication.receiveByte()) != ETB) {
    the_header->length = (the_header->length * 10) + (digit - '0');
  }

  if (the_header->length <= HEXFILE_MAX_SIZE) {
    Communication.sendByte(ACK);
  } else {
    Communication.sendByte(NAK);
    command = CMD_ERROR;
  }

  return command;
}

void receive_data(unsigned long data_length) {
  byte recv;
  do {
    recv = Communication.receiveByte();
  } while (recv != STX);

  unsigned int address = 0;

  // first read the RESB vector
  resb_lo = Communication.receiveByte();
  Communication.sendByte(resb_lo);

  resb_hi = Communication.receiveByte();
  Communication.sendByte(resb_hi);

  // write the RESB vector
  Memory.writeByte(0xFFFC, resb_lo);
  Memory.writeByte(0xFFFD, resb_hi);

  for (int i = 0; i < data_length - 2; i++) {
    recv = Communication.receiveByte();
    Memory.writeByte(address, recv);

    // send data back, to check
    byte data = Memory.readByte(address);
    Communication.sendByte(data);

    address++;
  }

  do {
    recv = Communication.receiveByte();
  } while (recv != ETX);
}

void send_data(unsigned int data_length) {
  Communication.sendByte(STX);

  for (unsigned int address = 0; address < data_length; address++) {
    byte data = Memory.readByte(address);
    Communication.sendByte(data);
  }

  Communication.sendByte(ETX);
}

void loop() {
  header the_header;
  idle();
  byte command = receive_header(&the_header);

  if (command == CMD_WRITE_EEPROM) {
    // the first two byte of the HEX file
    // are the lo and hi bytes of the first opcode
    // (the RESB vector); they will be valid after
    // "receive_data"
    receive_data(the_header.length);
  } else if (command == CMD_READ_EEPROM) {
    send_data(the_header.length);
  }

  Communication.sendByte(EOT);
}
