#include <stdio.h>
#include "generated/parser.h"
#include "identifier.h"
#include "tree.h"
#include "opcode.h"
#include <argp.h>
#include <string.h>
#include "options.h"


extern int linecounter;
extern int lexerrorcounter;
extern FILE *yyin;
int errors = 0;

void yyerror(char *s) {
    fprintf(stderr, "%s in line %d.\n", s, linecounter);
    errors++;
}




int main(int argc, char **argv) {
    FILE *asm_input = stdin;
    FILE *hex_output = stdout;

    struct arguments arguments = arguments_parse(argc, argv);

    if (arguments.output_file == NULL) {
        printf("You must provide an output file (-o, --output).\n");
        return -1;
    }

    if (arguments.input_file == NULL) {
        printf("You must provide an input file (-i, --input).\n");
        return -1;
    }

    asm_input = fopen(arguments.input_file, "r");
    if (!asm_input) {
        printf("Failed to open %s for reading.\n", arguments.input_file);
        return -1;
    }
    printf("Reading from %s\n", arguments.input_file);
    

    hex_output = fopen(arguments.output_file, "w");
    if (!hex_output) {
        printf("Failed to open %s for writing.\n", arguments.output_file);
        return -1;
    }
    printf("Writing to %s\n", arguments.output_file);
    
    yyin = asm_input;
    yyparse();

    fclose(asm_input);

    struct node *ptr = tree_head();
    while (ptr) {
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

                    fprintf(stderr, "addr: %s %c (%d) --> %04x oper len=%d, opcode=%d\n", 
                        ptr->operand.str, ptr->operand.operation, ptr->operand.offset, 
                        operand, operand_len, opcode);

                    // when no operand, we are done
                    if (operand_len == 0) {
                        break;
                    }

                    if (operand_len == 1) {
                        fprintf(hex_output, "%c", operand & 0x00ff);
                    }

                    if (operand_len == 2) {
                        fprintf(hex_output, "%c%c", operand & 0x00ff, operand >> 8);
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