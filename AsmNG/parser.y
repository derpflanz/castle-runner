%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int yylex(void);
void yyerror(char *);

int linecounter = 1;
unsigned short address = 0x0000;

struct element {
    char *name;
    unsigned short address;

    struct element *next;
};
struct element *identifiers = NULL;

enum addressing_mode {
    i = 0,
    accu = 1,
    zeropage = 2,
    absolute = 3
};

struct opcode {
    char *mnemonic;
    unsigned char opcode;
    enum addressing_mode mode;
};

struct opcode opcodes[4] = {
    {"lda", 0x00, i}, {"lda", 0x00, accu},
    {"lda", 0x00, i}, {"lda", 0x00, accu}
};

/* implied addressing */
void implied(char *mnemonic) {
    printf("[%04x] i    %s\n", address, mnemonic);

    address += 1;
    free(mnemonic);
}

/* direct addressing, can be zero page or absolute */
void direct(char *mnemonic, unsigned short operand) {
    if ((operand | 0x00ff) == 0x00ff) {
        printf("[%04x] zp   %s $%02x\n", address, mnemonic, operand);
        address += 2;
    } else {
        printf("[%04x] a    %s $%04x\n", address, mnemonic, operand);
        address += 3;
    }
    free(mnemonic);
}

void print_list(struct element *list) {
    printf("Current list:\n");

    struct element *p = list;

    while (p) {
        printf("element: name=%s, addr=%04x\n", p->name, p->address);
        p = p->next;
    }
}

void register_identifier(char *ident, unsigned short addr) {
    struct element *new_element = malloc(sizeof(struct element));
    new_element->name = strdup(ident);
    new_element->address = addr;
    new_element->next = identifiers;
    identifiers = new_element;

    free(ident);
}

void directive(char *directive, unsigned short addr) {
    if (strncmp("orig", directive, 4)) {
        address = addr;
    }

    free(directive);
}

void initialize() {
    printf("Initializing assembler.\n");
}

unsigned short get_address(char *ident) {
    struct element *p = identifiers;

    while (p) {
        if (!strncmp(ident, p->name, strlen(ident))) {
            return p->address;
        }
        p = p->next;
    }
}

%}

%token MNEMONIC
%token ABSOLUTE
%token COMMAX
%token IDENTIFIER
%token DIRECTIVE
%token ZEROPAGE

%union {
    char *str;
    unsigned short addr;
}

%type<str> MNEMONIC
%type<str> DIRECTIVE
%type<str> IDENTIFIER
%type<addr> ABSOLUTE
%type<addr> ZEROPAGE
%type<addr> zp_abs_identifier
%type<addr> zp_abs

%%

program:
    program expression '\n' { linecounter++; }
|   program '\n'            { linecounter++; }
|	program error '\n'		{ yyerrok; }
|   /* NOTHING */
;

expression:
    MNEMONIC { implied($1); }
|   MNEMONIC zp_abs_identifier { direct($1, $2); }
|   IDENTIFIER '=' zp_abs { register_identifier($1, $3); }
|   DIRECTIVE ABSOLUTE { directive($1, $2); }
;

zp_abs:
    ZEROPAGE
|   ABSOLUTE
;

zp_abs_identifier:
    ZEROPAGE
|   ABSOLUTE
|   IDENTIFIER  { $$ = get_address($1); }
;

%%

