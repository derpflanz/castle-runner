#ifndef __OPCODE_H
#define __OPCODE_H

#include <stdbool.h>
#include "generated/parser.h"
#include "identifier.h"
#include <string.h>

#define OPCODE_COUNT 256

bool opcode_lookup(const char *mnemonic, const char *mode, unsigned char *opcode);
int get_statement_length(const char *addressing_mode);
int get_operand_length(const unsigned char opcode);
unsigned short calculate_operand(const struct operand operand);

#endif