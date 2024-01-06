#include "memory.h"
#include "communication.h"
#include <Arduino.h>

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

uint16_t get_decimal(byte separator) {
  byte digit = 0;
  uint16_t result = 0;

  while ( (digit = Communication.receiveByte()) != separator) {
    result = (result * 10) + (digit - '0');
  }

  return result;
}

uint16_t get_address(byte separator) {
  byte digit = 0;
  uint16_t result = 0;
  while ( (digit = Communication.receiveByte()) != separator) {
    if (digit >= '0' && digit <= '9')
      result = (result * 16) + (digit - '0');

    digit |= 0x20;      // make lower case
    if (digit >= 'a' && digit <= 'f') 
      result = (result * 16) + (digit - 'a') + 10;
  }

  return result;
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

  the_header->start_address = get_address(US);
  the_header->length = get_decimal(US);
  the_header->resb = get_address(US);
  the_header->irq = get_address(ETB);
 
  if ((the_header->start_address + the_header->length) <= HEXFILE_MAX_SIZE) {
    Communication.sendByte(ACK);
  } else {
    Communication.sendByte(NAK);
    command = CMD_ERROR;
  }

  return command;
}

void receive_data(uint16_t start_address, unsigned long data_length) {
  byte recv;
  do {
    recv = Communication.receiveByte();
  } while (recv != STX);

  unsigned int address = start_address;

  for (int i = 0; i < data_length; i++) {
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

void write_vector(uint16_t vector, uint16_t address) {
  // the vector is a uint, so we need to make this little-endian again
  byte vector_lo = vector & 0x00FF;
  byte vector_hi = vector >> 8;

  Memory.writeByte(address, vector_lo);
  Memory.writeByte(address + 1, vector_hi);
}

void send_data(uint16_t start_addresss, unsigned int data_length) {
  Communication.sendByte(STX);

  for (unsigned int address = start_addresss; address < (start_addresss + data_length); address++) {
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
    // not the 0x7ffc/0x7ffe because we write onto a 32kB EEPROM
    // these addresses translate to 0xfffc and 0xfffe on the 6502
    // because the RAM has addresses 0x000-0x7fff
    write_vector(the_header.resb, 0x7ffc);
    write_vector(the_header.irq, 0x7ffe);
    receive_data(the_header.start_address, the_header.length);
  } else if (command == CMD_READ_EEPROM) {
    send_data(the_header.start_address, the_header.length);
  }

  Communication.sendByte(EOT);
}
