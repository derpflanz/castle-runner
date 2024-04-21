#include <stdio.h>
#include "generated/parser.h"
#include "identifier.h"
#include "tree.h"
#include "opcode.h"

extern int linecounter;
extern int lexerrorcounter;
int errors = 0;

void yyerror(char *s) {
    fprintf(stderr, "%s in line %d.\n", s, linecounter);
    errors++;
}

unsigned short calculate_operand(const struct operand operand) {
    unsigned short address = 0x0000;

    if (operand.str == NULL) {
        return address;
    }

    if (!get_address(operand.str, &address)) {
        address = strtol(operand.str+1, NULL, 16);
    }
    address += operand.offset;

    switch (operand.operation) {
        case '>':
            address = address >> 8;
        break;
        case '<':
            address = address & 0x00ff;
        break;
    }

    return address;
}

int main(int argc, char **argv) {
    FILE *hex_output = stdout;

    yyparse();

    struct node *ptr = tree_head();
    while (ptr->next) {
        switch(ptr->type) {
            case t_byte:
                // ptr->bytes holds "$xx"
                unsigned char value = (unsigned char)strtol((ptr->bytes)+1, NULL, 16);
                fprintf(hex_output, "%c", value);
            break;
            case t_string:
                fprintf(hex_output, "%s%c", ptr->bytes, '\0');
            break;
            case t_opcode:
                unsigned char opcode = 0x00;
                if (opcode_lookup(ptr->bytes, ptr->operand.addressing_mode, &opcode) == true) {
                    // output opcode byte
                    fprintf(hex_output, "%c", opcode);

                    unsigned short operand = calculate_operand(ptr->operand);
                    int operand_len = get_operand_length(opcode);

                    fprintf(stderr, "addr: %s %c (%d) --> %04x\n", ptr->operand.str, ptr->operand.operation, ptr->operand.offset, operand);
                    // when no operand, we are done
                    if (operand_len == 0) {
                        break;
                    }

                    if (operand_len == 1) {
                        fprintf(hex_output, "%c", operand & 0x00ff);
                    }

                    if (operand_len == 2) {

                    }

                    
                } else {
                    errors++;
                    fprintf(stderr, "Opcode lookup failed for '%s' and addressing mode '%s'.\n", ptr->bytes, ptr->operand.addressing_mode);
                }
            break;
        }

        ptr = ptr->next;
    }

    if (errors + lexerrorcounter > 0) {
        fprintf(stderr, "Parsing failed with %d error(s).\n", errors + lexerrorcounter);
        return 1;
    }

    return 0;
}