#include <math.h>
#include <stdint.h>
#include <stdio.h>

int fsample = 20000;
int fsignal = 440;

int main(int argc, char **argv) {
    double tsample = (1.0) / fsample;
    double b = 2 * M_PI * fsignal;

    unsigned char x = 0;

    uint16_t output = 250;
    uint16_t wout = 0;

    wout = 1 * output;
    printf("%d\n", (uint8_t)(wout >> 8));

    return 0;
}