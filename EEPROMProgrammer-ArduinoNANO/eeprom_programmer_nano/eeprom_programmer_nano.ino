// Address pins (A0 - A7; A8 - A15 are connected to GND now)
const char ADDR[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
const char DATA[] = { 8, 9, 10, 11, 12, 13, 14, 15 };

#define OE 21
#define WE 20
#define CE 19
#define FF_CLK 18
#define PRINTLINE Serial.println(linebuffer)

char linebuffer[80];

void SetDataToInput() {
  for (int i = 0; i <= 7; i++) {
    pinMode(DATA[i], INPUT);
  }
}

void SetDataToOutput() {
  for (int i = 0; i <= 7; i++) {
    pinMode(DATA[i], OUTPUT);
  }
}

void SetAddress(unsigned int address) {
  byte addr_lo = address & 0x00FF;
  byte addr_hi = address >> 8;
  byte mask = 0x01;
  digitalWrite(FF_CLK, LOW);
  
  sprintf(linebuffer, "Setting address 0x%04x, LO=0x%02x, HI=0x%02x", address, addr_lo, addr_hi);
  PRINTLINE;

  sprintf(linebuffer, "Clocking in HI byte first: 0x%02x", addr_hi);
  PRINTLINE;
  for (int i = 0; i <= 7; i++) {
    // sprintf(linebuffer, "Setting address pin %d to %d", ADDR[i], (addr_hi & mask)>0?HIGH:LOW);
    // PRINTLINE;
    
    digitalWrite(ADDR[i], (addr_hi & mask)>0?HIGH:LOW);
    mask <<= 1;
  }
  
  digitalWrite(FF_CLK, HIGH);   // this clocks in the high byte
  delayMicroseconds(10);
  digitalWrite(FF_CLK, LOW);

  sprintf(linebuffer, "Clocking in LO byte next: 0x%02x", addr_lo);
  PRINTLINE;

  mask = 0x01;
  for (int i = 0; i <= 7; i++) {
    digitalWrite(ADDR[i], (addr_lo & mask)>0?HIGH:LOW);
    mask <<= 1;
  }

  sprintf(linebuffer, "Done. Address set to 0x%04x", address);
}

void SetData(int data) {
  int mask = 0x01;
  
  sprintf(linebuffer, "Setting data %d", data);
  PRINTLINE;

  for (int i = 0; i <= 7; i++) {
    digitalWrite(DATA[i], (data & mask)>0?HIGH:LOW);
    mask <<= 1;
  }
}

void WriteROM(unsigned int address, int value) {
  digitalWrite(OE, HIGH);
  
  // setup address and data
  SetAddress(address);
  SetDataToOutput();
  SetData(value);
  
  delayMicroseconds(1);   // T_oes
  
  digitalWrite(CE, LOW);
  delayMicroseconds(1);   // T_cs
  
  digitalWrite(WE, LOW);
  delayMicroseconds(1);    // T_wp
  
  digitalWrite(WE, HIGH);
  delayMicroseconds(1);    // T_ch
  digitalWrite(CE, HIGH);
  
  delayMicroseconds(100);   // wait until write done
}

void WriteRAM(unsigned int address, int value) {
  digitalWrite(OE, HIGH);
  
  SetAddress(address);
  SetDataToOutput();
  SetData(value);
  
  // Write the data to the memory by pulling !WE LOW for at least 55ns (haha!)
  Serial.println("Writing data!");
  digitalWrite(CE, LOW);
  delayMicroseconds(1);
  digitalWrite(WE, LOW);
  delayMicroseconds(1);
  digitalWrite(WE, HIGH);
  digitalWrite(CE, HIGH);
}

int Read(unsigned int address) {
  SetAddress(address);
  SetDataToInput();
  
  Serial.println("Reading data!");
  digitalWrite(CE, LOW);
  delayMicroseconds(1);
  digitalWrite(OE, LOW);
  delayMicroseconds(1);
  byte data = GetData();
  sprintf(linebuffer, "Got data: %c (%d, 0x%04x)", data, data, data);
  PRINTLINE;
  delayMicroseconds(1);
  digitalWrite(OE, HIGH);
  digitalWrite(CE, HIGH);
  
  return data;
}


byte GetData() {
  byte data = 0;
  for (int i = 7; i >= 0; i--) {
    byte bit = digitalRead(DATA[i]) ? 1 : 0;
    //sprintf(linebuffer, "Read bit #%d: %d", i, bit);
    //PRINTLINE;
    data = (data << 1) | bit;
  }

  return data;
}


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  
  Serial.println("SRAM test program v1");
  
  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(CE, OUTPUT);
  pinMode(FF_CLK, OUTPUT);

  Serial.println("Pulling !CE HIGH");
  digitalWrite(CE, HIGH);
  
  Serial.println("Pulling !WE HIGH, to prevent accidental writes");
  digitalWrite(WE, HIGH);
  
  Serial.println("Pulling !OE HIGH, to put chip in HIGH-Z");
  digitalWrite(OE, LOW);

  digitalWrite(FF_CLK, LOW);
  
  Serial.println("Setting address lines to OUTPUT");
  for (int i = 0; i <= 7; i++) {
    pinMode(ADDR[i], OUTPUT);
  }
  
  // Set to High-Z
  SetDataToInput();

  demo();
}


void debug() {
  SetAddress(0x0001);
}

void demo() {  
  WriteROM(0x4010, 'A');
  WriteROM(0x6020, 'B');
  
  int result = Read(0x4010);
  int result2 = Read(0x6020);

  sprintf(linebuffer, "Done: %c (%d)", result, result);  
  PRINTLINE;

  sprintf(linebuffer, "Done: %c (%d)", result2, result2);  
  PRINTLINE;

}

// the loop function runs over and over again forever
void loop() {
}
