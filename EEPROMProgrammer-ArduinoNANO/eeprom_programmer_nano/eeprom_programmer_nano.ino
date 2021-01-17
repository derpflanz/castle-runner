#include "memory.h"
#include "communication.h"

void setup() {
  Communication.setup();
  Memory.setup();

  Communication.printf("EEPROM Programmer v2");
  Communication.sendByte(SYN);
}

void idle() {
  byte recv;
  do {
    recv = Communication.receiveByte();
  } while (recv != SOH);
}

byte receive_header(unsigned long *data_length) {
  byte digit, command = CMD_NOTHING;
  *data_length = 0;
  command = Communication.receiveByte();

  while ( (digit = Communication.receiveByte()) != ETB) {
    *data_length = (*data_length * 10) + (digit - '0');
  }

  if (*data_length < HEXFILE_MAX_SIZE) {
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
  for (int i = 0; i < data_length; i++) {
    recv = Communication.receiveByte();
    Memory.writeByte(address, recv);
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
  unsigned long data_length = 0;
  idle();
  byte command = receive_header(&data_length);

  if (command == CMD_WRITE_EEPROM) {
    receive_data(data_length);
  } else if (command == CMD_READ_EEPROM) {
    send_data(data_length);
  }

  Communication.sendByte(EOT);
}
