
const char ADDR[]  = { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45 };
extern volatile unsigned long timer0_overflow_count;
unsigned long ticks() { return timer0_overflow_count; }
unsigned char shadow[3];

void setup() {
  for (int i = 0; i < 24; i++) {
    pinMode(ADDR[i], INPUT_PULLUP);
  }

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Monitor Tool, for Arduino MEGA");
  Serial.println("This is a 24-bit data monitor.");
  Serial.println("(c) 2021 BF");
}

void printBinary(unsigned char b) {
  unsigned char mask = 0x80;
  for (char i = 0; i < 8; i++) {
    if (i == 4) Serial.write('.');
    Serial.write((b & mask)?'1':'0');
    mask >>= 1;
  }
}

void printHexadecimal(unsigned char b) {
  if (b < 0x10) {
    Serial.write('0');
  }
  Serial.print(b, HEX);
}

void printAscii(unsigned char b) {
  if (b >= 0x20 && b <= 0x7E) {
    Serial.print("  ");
    Serial.write(b);
  } else {
    Serial.print("UNK");
  }
}

void printData() {
  // 24 bits = 6 nibbles = 3 bytes
  // 0000 0000 0000 0000 0000 0000
  //    X    X    X    X    X    X
  //         A         A         A

  unsigned char data[3] = {0, 0, 0};
  unsigned char mask = 0x01;
  
  for (int i = 0; i < 24; i++) {
    char pinState = digitalRead(ADDR[i]);
    if (pinState == HIGH) {
      data[2 - (i / 8)] = data[2 - (i / 8)] | mask;
    }

    if (mask == 0x80) {
      mask = 0x01;
    } else {
      mask <<= 1;
    }
  }

  if (shadow[0] != data[0] || shadow[1] != data[1] || shadow[2] != data[2]) {
    printBinary(data[0]);
    Serial.print(" ");
    printBinary(data[1]);
    Serial.print(" ");
    printBinary(data[2]);
    Serial.print("  ");

    printHexadecimal(data[0]);
    Serial.print(" ");
    printHexadecimal(data[1]);
    Serial.print(" ");
    printHexadecimal(data[2]);
    Serial.print("  ");

    printAscii(data[0]);
    Serial.print(" ");
    printAscii(data[1]);
    Serial.print(" ");
    printAscii(data[2]);
    Serial.print("  ");

    
    Serial.println();
    shadow[0] = data[0];
    shadow[1] = data[1];
    shadow[2] = data[2];
  }
}

void onClock() {
}

void loop() {
  delayMicroseconds(1);
  printData();
}
