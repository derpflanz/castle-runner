#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "Arduino.h"

// pins
#define OE 21
#define WE 20
#define CE 19
#define FF_CLK 18

class EepromMemory {
  public:
    void setup();

  private:
    void SetDataToInput();
    void SetDataToOutput();
    void SetAddress(unsigned int address);
    void SetData(int data);
    void Write(unsigned int address, int value);
    int Read(unsigned int address);
    byte GetData();
};

extern EepromMemory Memory;

#endif