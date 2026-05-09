#include "memory.h"
#include <Arduino.h>

// Address pins (A0 - A7; A8 - A15 are connected to GND now)
const char ADDR[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
const char DATA[] = { 8, 9, 10, 11, 12, 13, 14, 15 };

void EepromMemory::setup() {
    // configure control pins
  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(CE, OUTPUT);
  pinMode(FF_CLK, OUTPUT);

  // set control pins
  digitalWrite(CE, HIGH);
  digitalWrite(WE, HIGH);  
  digitalWrite(OE, LOW);      // put chip in High-Z
  digitalWrite(FF_CLK, LOW);

  // configure address pins
  for (int i = 0; i <= 7; i++) {
    pinMode(ADDR[i], OUTPUT);
  }
  
  SetDataToInput();
}

void EepromMemory::SetDataToInput() {
  for (int i = 0; i <= 7; i++) {
    pinMode(DATA[i], INPUT);
  }
}

void EepromMemory::SetDataToOutput() {
  for (int i = 0; i <= 7; i++) {
    pinMode(DATA[i], OUTPUT);
  }
}

void EepromMemory::SetAddress(unsigned int address) {
  byte addr_lo = address & 0x00FF;
  byte addr_hi = address >> 8;
  byte mask = 0x01;
  digitalWrite(FF_CLK, LOW);

  // set high byte on address outputs
  for (int i = 0; i <= 7; i++) {    
    digitalWrite(ADDR[i], (addr_hi & mask)>0?HIGH:LOW);
    mask <<= 1;
  }

  // clock in high byte on the flip-flop
  digitalWrite(FF_CLK, HIGH);
  delayMicroseconds(10);
  digitalWrite(FF_CLK, LOW);

  // set low byte on address outputs
  mask = 0x01;
  for (int i = 0; i <= 7; i++) {
    digitalWrite(ADDR[i], (addr_lo & mask)>0?HIGH:LOW);
    mask <<= 1;
  }
}

void EepromMemory::SetData(int data) {
  int mask = 0x01;
  
  for (int i = 0; i <= 7; i++) {
    digitalWrite(DATA[i], (data & mask)>0?HIGH:LOW);
    mask <<= 1;
  }
}

void EepromMemory::writeByte(unsigned int address, byte value) {
  unsigned int currentPageAddr = address & 0xFFC0;  // page address (A6-A14)
  unsigned int offsetInPage = address & 0x003F;     // offset within page (A0-A5)

  // If we're switching to a new page, program the current buffer first
  if (bufferPageAddr != 0xFFFF && currentPageAddr != bufferPageAddr) {
    programPage();
  }

  // Start a new page buffer if needed
  if (bufferPageAddr != currentPageAddr) {
    bufferPageAddr = currentPageAddr;
    bufferCount = 0;
  }

  // Add byte to buffer
  writeBuffer[offsetInPage] = value;
  bufferCount++;

  // Auto-flush if buffer is full (all 64 bytes of page written)
  if (bufferCount == PAGE_SIZE) {
    programPage();
  }
}

void EepromMemory::programPage() {
  if (bufferCount == 0) return;  // nothing to program

  digitalWrite(OE, HIGH);
  digitalWrite(WE, HIGH);
  digitalWrite(CE, HIGH);

  // Load all buffered bytes into the chip's internal buffer
  for (int i = 0; i < bufferCount; i++) {
    unsigned int address = bufferPageAddr + i;
    
    SetAddress(address);
    SetDataToOutput();
    SetData(writeBuffer[i]);
    
    delayMicroseconds(1);   // setup time
    
    digitalWrite(CE, LOW);
    delayMicroseconds(1);
    
    // Pulse WE to latch byte into chip's buffer
    digitalWrite(WE, LOW);
    delayMicroseconds(1);    // WE pulse width
    digitalWrite(WE, HIGH);
    delayMicroseconds(1);
    
    digitalWrite(CE, HIGH);
    delayMicroseconds(1);
  }

  // Now program the entire page with final WE pulse
  digitalWrite(CE, LOW);
  delayMicroseconds(1);
  
  digitalWrite(WE, LOW);
  delayMicroseconds(1);
  
  digitalWrite(WE, HIGH);
  delayMicroseconds(1);
  
  digitalWrite(CE, HIGH);
  
  // Wait for page programming to complete
  delay(10);
  
  // Reset buffer
  bufferPageAddr = 0xFFFF;
  bufferCount = 0;
}

void EepromMemory::flushPageBuffer() {
  programPage();
}

byte EepromMemory::readByte(unsigned int address) {
  digitalWrite(OE, HIGH);
  digitalWrite(WE, HIGH);
  digitalWrite(CE, HIGH);

  SetAddress(address);
  SetDataToInput();
  
  digitalWrite(CE, LOW);
  delayMicroseconds(1);
  digitalWrite(OE, LOW);
  delayMicroseconds(1);
  byte data = GetData();
  
  delayMicroseconds(1);
  digitalWrite(OE, HIGH);
  digitalWrite(CE, HIGH);
  
  return data;
}

byte EepromMemory::GetData() {
  byte data = 0;
  for (int i = 7; i >= 0; i--) {
    byte bit = digitalRead(DATA[i]) ? 1 : 0;
    data = (data << 1) | bit;
  }

  return data;
}

EepromMemory Memory;