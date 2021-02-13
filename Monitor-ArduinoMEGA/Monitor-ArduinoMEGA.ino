
const char ADDR[]  = { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45 };
const char LEDS[] = { 4, 5, 6, 7, 8, 9, 10, 11 };
const char OUT2[] = { 46, 47, 48 };

#define CLOCK   2
#define R_NOTW  3
#define CE      37        // pin 37 = A15, which is used as chip enable
#define PRINT { Serial.print(linebuffer); }

unsigned char shadow[3];
unsigned int i = 0;
char linebuffer[80];

void writeLeds(byte pattern) {
  byte mask = 0x01;

  for (int i = 0; i < 8; i++) {
    if ((pattern & mask) > 0) {
      digitalWrite(LEDS[i], HIGH);
    } else {
      digitalWrite(LEDS[i], LOW);
    }
    mask <<= 1;
  }
}

void writeOut2(byte pattern) {
  if (pattern & 0x01) {
    // enable
    digitalWrite(OUT2[0], HIGH);
  } else {
    digitalWrite(OUT2[0], LOW);
  }

  if (pattern & 0x02) {
    // rw
    digitalWrite(OUT2[1], HIGH);
  } else {
    digitalWrite(OUT2[1], LOW);
  }

  if (pattern & 0x04) {
    // RS
    digitalWrite(OUT2[2], HIGH);
  } else {
    digitalWrite(OUT2[2], LOW);
  }

}

void setup() {
  for (int i = 0; i < 24; i++) {
    pinMode(ADDR[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 8; i++) {
    pinMode(LEDS[i], OUTPUT);
  }
  for (int i = 0; i < 3; i++) {
    pinMode(OUT2[i], OUTPUT);
  }

  writeLeds(0x55);
  delay(500);
  writeLeds(0xAA);
  delay(1000);
  writeLeds(0x55);
  delay(500);
  writeLeds(0x00);

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Monitor Tool, for Arduino MEGA");
  Serial.println("This is a 24-bit data monitor.");
  Serial.println("(c) 2021 BF");

  pinMode(CLOCK, INPUT);
  pinMode(R_NOTW, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);
}

static void onClock() {
  printData(false);
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

void printRW() {
  int rw = digitalRead(R_NOTW);
  sprintf(linebuffer, "R/!W = %d %s", rw, rw==HIGH?"READ ":"WRITE");
  PRINT;
}

void printCE() {
  int ce = digitalRead(CE);
  sprintf(linebuffer, "CE = %d %s", ce, ce==LOW?"RAM":"ROM");
  PRINT;
}

void printData(char useShadow) {
  // 24 bits = 6 nibbles = 3 bytes
  // 0000 0000 0000 0000 0000 0000
  //    X    X    X    X    X    X
  //         A         A         A

  byte data[3] = {0, 0, 0};
  byte mask = 0x01;
  
  for (int i = 0; i < 24; i++) {
    int pinState = digitalRead(ADDR[i]);
    if (pinState == HIGH) {
      data[2 - (i / 8)] = data[2 - (i / 8)] | mask;
    }

    if (mask == 0x80) {
      mask = 0x01;
    } else {
      mask <<= 1;
    }
  }

  // data[0] = databus
  // data[1] = address HIGH
  // data[2] = address LOW
  if (!useShadow || (shadow[0] != data[0] || shadow[1] != data[1] || shadow[2] != data[2])) {
    Serial.print("address: ");
    printRW();
    Serial.print("  ");    

    printHexadecimal(data[1]);
    Serial.print(" ");
    printHexadecimal(data[2]);
    Serial.print("  ");

    printBinary(data[1]);
    Serial.print(" ");
    printBinary(data[2]);
    Serial.print("  ");

    // we also have some extra pins
    printCE();
    Serial.print("  ");

    Serial.print("data: ");    
    printBinary(data[0]);
    Serial.print("  ");
    printHexadecimal(data[0]);
    Serial.print("  ");
    printAscii(data[0]);
    Serial.print("   ");

    if (data[1] == 0x7F && data[2] == 0xFA) {
      writeLeds(data[0]);
      Serial.print("I/O 1: ");
      printHexadecimal(data[0]);
    }
    if (data[1] == 0x7F && data[2] == 0xFB) {
      writeOut2(data[0]);
      Serial.print("I/O 2: ");
      printHexadecimal(data[0]);
    }



    Serial.println();
    Serial.flush();

    shadow[0] = data[0];
    shadow[1] = data[1];
    shadow[2] = data[2];
  }
}

void loop() {
}
