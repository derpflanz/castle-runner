#include "clock.h"
#include "log.h"

static void onManual() {
    Clock.manual();
}

void ClockGenerator::setup() {
    // setup I/O
    pinMode(CLOCK_MANUAL, INPUT_PULLUP);
    pinMode(CLOCK_OUT, OUTPUT);

    // initialise
    digitalWrite(CLOCK_OUT, HIGH);
    counter = 0;
    speed = analogRead(CLOCK_SENSOR);
    timestamp = millis();
    interrupt = NULL;

    // attach interupts
    attachInterrupt(digitalPinToInterrupt(CLOCK_MANUAL), onManual, RISING);

    Log.println("Clock setup.");
}

void ClockGenerator::loop() {
    // reset counter and trigger tick
    if (counter == 0) {
        counter = (1024 - speed);
        external_tick();
    }

    // countdown the counter
    if (speed > 0) {
        delayMicroseconds(1);
        counter--;
        if (counter < 0) counter = 0;
    }

    // read speed (we only need to do this once every few seconds)
    if ((millis() - timestamp) > 5000) {
        timestamp = millis();
        speed = analogRead(CLOCK_SENSOR);
    }
}

void ClockGenerator::manual() {
    if (speed == 0) {
        external_tick();        
    }
}

void ClockGenerator::external_tick() {
    if (interrupt) interrupt(speed);

    digitalWrite(CLOCK_OUT, LOW);
    delayMicroseconds(1);
    digitalWrite(CLOCK_OUT, HIGH);
}

void ClockGenerator::onTick(void (*intr)(int)) {
    interrupt = intr;
}

ClockGenerator Clock;