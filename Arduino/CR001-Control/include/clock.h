#ifndef __CLOCK_H
#define __CLOCK_H

// CLOCK is an OUTPUT (We control the board from the Arduino)
#define CLOCK_OUT       7

// The analog sensor, used to control speed
#define CLOCK_SENSOR    A0
#define CLOCK_MANUAL     2

class ClockGenerator {
    private:
        void (*interrupt)(int);
        int counter;
        int speed;
        unsigned long timestamp;

        void external_tick();
    public:
        void setup();
        void loop();
        void manual();

        void onTick(void (*intr)(int));
};

extern ClockGenerator Clock;

#endif