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
                    unsigned short address = 0x0000;
                    if (ptr->operand.str == NULL) {
                        fprintf(hex_output, "%c", opcode);
                        break;
                    } 
                    
                    if (!get_address(ptr->operand.str, &address)) {
                        address = strtol((ptr->operand.str)+1, NULL, 16);
                    }

                    address += ptr->operand.offset;

                    fprintf(stderr, "addr: %s (%d) --> %04x\n", ptr->operand.str, ptr->operand.offset, address);
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