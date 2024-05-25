#include <stdio.h>
#include "generated/parser.h"
#include "identifier.h"
#include "tree.h"
#include "opcode.h"
#include <argp.h>
#include <string.h>
#include "options.h"
#include "ignores.h"

#define COL_WIDTH   30

#define E_ARG       1
#define E_PARSE     2
#define E_VECTOR    3
#define E_PROCESS   4
#define E_OK        0

extern int linecounter;
extern int lexerrorcounter;
extern FILE *yyin;
int errors = 0;

void yyerror(char *s) {
    fprintf(stderr, "%s in line %d.\n", s, linecounter);
    errors++;
}

void hex_print(FILE *output, const char *s) {
    while (*s) fprintf(output, "%02x ", *s++);
    fprintf (output, "%02x", '\0');
}

void address_print(FILE *output, unsigned short address) {
    fprintf(output, "[%04x %-20s] ", address, identifier_reverse_lookup(address));
}

bool open_files(struct arguments *args, FILE **asm_input, FILE **hex_output) {
    *asm_input = fopen(args->input_file, "r");
    if (!*asm_input) {
        printf("Failed to open %s for reading.\n", args->input_file);
        return false;
    }
    printf("Reading from %s\n", args->input_file);
    
    *hex_output = fopen(args->output_file, "w");
    if (!*hex_output) {
        printf("Failed to open %s for writing.\n", args->output_file);
        return false;
    }
    printf("Writing to %s\n", args->output_file);

    return true;
}

bool get_vectors(unsigned short *resb, unsigned short *irq) {
    if (identifier_get("HW_IRQ", irq) == false) {
        printf("No IRQ vector found. Cannot write hex file.\n");
        return false;
    }

    struct node *ptr = tree_head();
    while (ptr) {
        if (ptr->type == t_opcode) {
            *resb = ptr->address;
            return true;
        }
        ptr = ptr->next;
    }

    printf("No RESB vector found. Cannot write hex file.\n");
    return false;
}

bool write_vectors(FILE *hex_output) {
    unsigned short resb, irq;

    if (!get_vectors(&resb, &irq)) {
        return false;
    }

    printf("Adding vectors to hex file: RESB=%04x, IRQ=%04x\n", resb, irq);
    fprintf(hex_output, "%c%c", resb & 0x00ff, resb >> 8);
    fprintf(hex_output, "%c%c", irq & 0x00ff, irq >> 8);

    return true;
}

void write_byte(FILE *hex_output, FILE *user_output, struct node *node) {
    // ptr->bytes holds "$xx"
    unsigned char value = (unsigned char)strtol((node->bytes)+1, NULL, 16);
    fprintf(hex_output, "%c", value);
    fprintf(user_output, ".byte %*s%x\n", -COL_WIDTH+6, node->bytes, value);
}

void write_string(FILE *hex_output, FILE *user_output, struct node *node) {
    fprintf(hex_output, "%s%c", node->bytes, '\0');
    fprintf(user_output, "%*s", -COL_WIDTH, node->bytes);
    hex_print(user_output, node->bytes);
    fprintf(user_output, "\n");
}

void write_opcode(FILE *hex_output, FILE *user_output, struct node *node, char **ignores) {
    unsigned char opcode = 0x00;
    if (opcode_lookup(node->bytes, node->operand.addressing_mode, &opcode) == true) {
        if (ignores_in_list(ignores, node->operand.str)) {
            // when an ignore is found as label, we assume it to be a JMP
            // so we replace this by three NOPs
            unsigned char nop;
            opcode_lookup("nop", "i", &nop);
            fprintf(hex_output, "%c%c%c", nop, nop, nop);
            fprintf(user_output, "%*s%02x %02x %02x", -COL_WIDTH, "nop nop nop", nop, nop, nop);
            fprintf(user_output, " (%s ignored)\n", node->operand.str);
            return;
        }

        // output opcode byte
        fprintf(hex_output, "%c", opcode);

        unsigned short operand = calculate_operand(node->operand);
        int operand_len = get_operand_length(opcode);

        // when no operand, we are done
        if (operand_len == 0) {
            fprintf(user_output, "%*s%02x", -COL_WIDTH, node->bytes, opcode);
        }

        if (operand_len >= 1) {
            fprintf(hex_output, "%c", operand & 0x00ff);
            fprintf(user_output, "%s %*s%02x %02x", 
                node->bytes, -COL_WIDTH+4, node->operand.str, opcode, operand & 0x00ff);
        }

        if (operand_len == 2) {
            fprintf(hex_output, "%c", operand >> 8);
            fprintf(user_output, " %02x", operand >> 8);
        }

        fprintf(user_output, "\n");
    } else {
        errors++;
        fprintf(stderr, "Opcode lookup failed for '%s' and addressing mode '%s'.\n", 
            node->bytes, node->operand.addressing_mode);
    }
}

int main(int argc, char **argv) {
    FILE *asm_input = stdin;
    FILE *hex_output = stdout;
    FILE *user_output = fopen("/dev/null", "w");
    char **ignores = NULL;

    struct arguments arguments;
    if (!arguments_parse(argc, argv, &arguments)) {
        return E_ARG;
    }

    if (!open_files(&arguments, &asm_input, &hex_output)) {
        return E_ARG;
    }

    if (arguments.show_result == 1) {
        user_output = stdout;
    }
    
    if (arguments.ignores != NULL) {
        ignores = ignores_create(arguments.ignores);
        printf("Going to ignore calls to the following labels:\n");
        ignores_print(ignores);
    }

    // Parse!
    yyin = asm_input;
    yyparse();
    fclose(asm_input);

    if (errors + lexerrorcounter > 0) {
        fprintf(stderr, "Parsing failed with %d error(s).\n", errors + lexerrorcounter);
        return E_PARSE;
    }

    if (arguments.add_vectors == 1) {
        if (!write_vectors(hex_output)) {
            return E_VECTOR;
        }
    }

    // Process!
    struct node *node = tree_head();
    while (node) {
        address_print(user_output, node->address);

        switch(node->type) {
            case t_byte:
                write_byte(hex_output, user_output, node);
            break;
            case t_string:
                write_string(hex_output, user_output, node);
            break;
            case t_opcode:
                write_opcode(hex_output, user_output, node, ignores);
            break;
        }

        node = node->next;
    }

    if (errors + lexerrorcounter > 0) {
        fprintf(stderr, "Processing failed with %d error(s).\n", errors + lexerrorcounter);
        return E_PROCESS;
    }

    ignores_free(ignores);

    return E_OK;
}