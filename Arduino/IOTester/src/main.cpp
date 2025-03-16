#include <Arduino.h>

#define START_PIN 2
#define BYTE_LENGTH 8

#define DATAREG     10
#define CTRLREG     11
#define WRITE       12

// The LEDBAR is LOW ACTIVE
// The Databus is HIGH ACTIVE
#define ACTIVE HIGH

void set_output() {
    for (int i = START_PIN; i < (START_PIN + BYTE_LENGTH); i++) {
        pinMode(i, OUTPUT);
    }

    pinMode(DATAREG, OUTPUT);
    pinMode(CTRLREG, OUTPUT);
    pinMode(WRITE, OUTPUT);
}

void write_data(uint8_t data) {
    uint8_t mask = 0x01;

    for (int i = START_PIN; i < (START_PIN + BYTE_LENGTH); i++) {
        if (mask & data) {
            digitalWrite(i, ACTIVE);
        } else {
            digitalWrite(i, !ACTIVE);
        }

        mask <<= 1;
    }
}

void set_data_register() {
    digitalWrite(DATAREG, ACTIVE);
    digitalWrite(CTRLREG, !ACTIVE);
}

void set_ctrl_register() {
    digitalWrite(CTRLREG, ACTIVE);
    digitalWrite(DATAREG, !ACTIVE);
}

void clear_register() {
    digitalWrite(CTRLREG, !ACTIVE);
    digitalWrite(DATAREG, !ACTIVE);
}

void read() {
    digitalWrite(WRITE, ACTIVE);
}

void write() {
    digitalWrite(WRITE, !ACTIVE);
    delay(500);
    digitalWrite(WRITE, ACTIVE);
}

void setup() {
    set_output();
    write_data(0);
    clear_register();
    read();

    Serial.begin(9600);
    Serial.write("\n\n\nWelcome to the CR002 IO Tester\n");
    Serial.write("O<number>     to set data output\n");
    Serial.write("D             to select data register\n");
    Serial.write("C             to select control register\n");
    Serial.write("X             to clear the data/ctrl registers\n");
    Serial.write("W             write out\n\n");
    Serial.write("> ");
}

void process(char *buffer) {
    if (buffer[0] == 'O') {
        char value = (char)atoi(&buffer[1]);
        write_data(value);

        char vbuffer[100];
        sprintf(vbuffer, "%d\n", value);
        Serial.write("Setting output to ");
        Serial.write(vbuffer);
    } else if (buffer[0] == 'D') {
        Serial.write("Enabled data register\n");
        set_data_register();
    } else if (buffer[0] == 'C') {
        Serial.write("Enabled control register\n");
        set_ctrl_register();
    } else if (buffer[0] == 'X') {
        Serial.write("Disabled both registers\n");
        clear_register();
    } else if (buffer[0] == 'W') {
        Serial.write("Edged in WR\n");
        write();
    } else {
        Serial.write("You send an unknown command: ");
        Serial.write(buffer);        
    }
}

void loop() {
    static char buffer[128];
    static int bufptr = 0;
    
    if (Serial.available()) {
        int received;
        received = Serial.read();
        Serial.write(received);
        buffer[bufptr++] = (char)received;
        if (received == '\n') {
            buffer[bufptr] = '\0';
            process(buffer);
            bufptr = 0;
            Serial.write("> ");
        }
    }

}

