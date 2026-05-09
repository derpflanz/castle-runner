#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <Arduino.h>

// pins
#define OE 21
#define WE 20
#define CE 19
#define FF_CLK 18

#define PAGE_SIZE 64

class EepromMemory {
  public:
    void setup();
    byte readByte(unsigned int address);
    void writeByte(unsigned int address, byte value);
    void flushPageBuffer();  // program any pending buffered writes

  private:
    void SetDataToInput();
    void SetDataToOutput();
    void SetAddress(unsigned int address);
    void SetData(int data);
    byte GetData();
    void programPage();  // issue the page write command

    // Page write buffer
    byte writeBuffer[PAGE_SIZE];
    unsigned int bufferPageAddr = 0xFFFF;  // current page address in buffer
    int bufferCount = 0;  // number of bytes in buffer
};

extern EepromMemory Memory;

#endif