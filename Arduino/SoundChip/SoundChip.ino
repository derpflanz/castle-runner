// SBY, PIN 8, Should be HIGH when we want to load an address
#define STANDBY     52

// !ALD, PIN 20, Edge a zero to load address (but only when STANDBY is HIGH)
#define ADDRLOAD    50

const int ADDR[8] = { 22, 24, 26, 28, 30, 32, 34, 36 };

// allophones
#define THREE 0x03

void setup() {
    Serial.begin(115200);
    Serial.println("SoundChip");

    pinMode(STANDBY, INPUT_PULLUP);
    pinMode(ADDRLOAD, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    for (int i = 0; i <= 7; i++) {
        pinMode(ADDR[i], OUTPUT);
        digitalWrite(ADDR[i], LOW);
    }

    digitalWrite(ADDRLOAD, HIGH);
}

void wait_for_standby() {
    int wait = 1;
    while (wait == 1) {
        char c = digitalRead(STANDBY);
        digitalWrite(LED_BUILTIN, c);
        
        if (c == HIGH) {
            wait = 0;
        }
    }

    Serial.println("Standing by");
}

void set_address(char allophone) {
    char mask = 0x01;
    for (int i = 0; i <= 7; i++) {
        if (mask & allophone) {            
            digitalWrite(ADDR[i], HIGH);
        } else {
            digitalWrite(ADDR[i], LOW);
        }

        mask <<= 1;
    }

    Serial.println("Set address");
}

void edge_address() {
    digitalWrite(ADDRLOAD, LOW);
    delay(1);
    digitalWrite(ADDRLOAD, HIGH);

    Serial.println("Edged!");
}

void say(char allophone) {
    wait_for_standby();
    set_address(allophone);
    edge_address();
}

void loop() {
    say(0x20);

    delay(2000);

    // for (char c = 0; c <= 250; c++) {
    //     if (c == 11) continue;
    //     say (c);
    //     delay(1000);
    // }

    // for (char c = 1; c <= 9; c++) {
    //     Serial.print(c);
    //     say(c);
    //     delay(1000);
    // }
}