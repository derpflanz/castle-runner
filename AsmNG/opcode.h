#ifndef __OPCODE_H
#define __OPCODE_H

#include <stdbool.h>

#define OPCODE_COUNT 256

bool opcode_lookup(const char *mnemonic, const char *mode, unsigned char *opcode);
int get_statement_length(const char *addressing_mode);

extern const char *addressing_modes[OPCODE_COUNT];
extern const char *mnemonics[OPCODE_COUNT];


#endif