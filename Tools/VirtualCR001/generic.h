#ifndef __GENERIC_H
#define __GENERIC_H

#include <stdint.h>

#define JOY_LEFT    247  // = F7 = 1111 0111 = bit 3 = 0x08 mask
#define JOY_RIGHT   239  // = EF = 1110 1111 = bit 4 = 0x10 mask
#define JOY_UP      253  // = FD = 1111 1101 = bit 1 = 0x02 mask
#define JOY_DOWN    251  // = FB = 1111 1011 = bit 2 = 0x04 mask
#define JOY_FIRE    191  // = BF = 1011 1111 = bit 6 = 0x40 mask

int array_contains(uint16_t needle, uint16_t *haystack);

#endif