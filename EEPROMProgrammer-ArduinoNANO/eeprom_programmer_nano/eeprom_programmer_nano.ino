// Address pins (A0 - A7; A8 - A15 are connected to GND now)
const char ADDR[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
const char DATA[] = { 8, 9, 10, 11, 12, 13, 14, 15 };

// pins
#define OE 21
#define WE 20
#define CE 19
#define FF_CLK 18
#define PRINTLINE { Serial.print("[CR-EP1] "); Serial.println(linebuffer); }

// constants
#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define ENQ 0x05
#define ACK 0x06

#define NAK 0x15

#define HEXFILE_MAX_SIZE  32768

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

void SetData(int data) {
  int mask = 0x01;
  
  for (int i = 0; i <= 7; i++) {
    digitalWrite(DATA[i], (data & mask)>0?HIGH:LOW);
    mask <<= 1;
  }
}

void Write(unsigned int address, int value) {
  digitalWrite(OE, HIGH);
  
  // setup address and data
  SetAddress(address);
  SetDataToOutput();
  SetData(value);
  
  delayMicroseconds(1);   // T_oes
  
  digitalWrite(CE, LOW);
  delayMicroseconds(1);   // T_cs

  // Write the data to the memory by pulling !WE LOW for at least 55ns (haha!)
  digitalWrite(WE, LOW);
  delayMicroseconds(1);    // T_wp
  
  digitalWrite(WE, HIGH);
  delayMicroseconds(1);    // T_ch
  digitalWrite(CE, HIGH);
  
  delayMicroseconds(100);   // wait until write done
}

int Read(unsigned int address) {
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


byte GetData() {
  byte data = 0;
  for (int i = 7; i >= 0; i--) {
    byte bit = digitalRead(DATA[i]) ? 1 : 0;
    data = (data << 1) | bit;
  }

  return data;
}


void setup() {
  Serial.begin(115200);

  sprintf(linebuffer, "EEPROM Programmer v1");
  PRINTLINE;

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

  // Send ENQ to tell our partner we are ready to receive data
  Serial.write(ENQ);
}


void demo() {  
  Write(0x4010, 'A');
  Write(0x6020, 'B');
  
  int result = Read(0x4010);
  int result2 = Read(0x6020);

  sprintf(linebuffer, "Done: %c (%d)", result, result);  
  PRINTLINE;

  sprintf(linebuffer, "Done: %c (%d)", result2, result2);  
  PRINTLINE;
}

// the loop function runs over and over again forever
void loop() {
  if (Serial.available()) {
    unsigned int size = 0;
    unsigned int counter = 0;
    int b = Serial.read();
    if (b == STX) {
      while ( (b = Serial.read()) != ETX) {
        if (b != -1) {
          size = (size * 10) + (b - 0x30);
        }
      }
    }

    if (size > HEXFILE_MAX_SIZE) {
      sprintf(linebuffer, "HEX file too large! Received %u, Max size is %u", size, HEXFILE_MAX_SIZE);
      PRINTLINE;
      Serial.write(NAK);
    } else {
      sprintf(linebuffer, "Going to read %d bytes from serial.", size);
      PRINTLINE;
      Serial.write(ACK);

      while (counter < size) {
        b = Serial.read();
        if (b != -1) {
          Write(counter, b);          
          counter++;
        }
      }

      b = Serial.read();
      if (b == EOT) {
        sprintf(linebuffer, "HEX reading successful.");
        PRINTLINE;
      }
    }
  }
}
