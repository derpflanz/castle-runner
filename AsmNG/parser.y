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

void processMnemonic(char *mn) {
    printf("[%04x] %s\n", address, mn);

    address += 1;
    free(mn);
}

void zpx(char *mn, unsigned short zp) {
    printf("[%04x] %s (%02x,X)\n", address, mn, zp);

    address += 2;

    free(mn);
}

void zp(char *mn, unsigned short zp) {
    printf("[%04x] %s %02x\n", address, mn, zp);

    address += 2;

    free(mn);
}

void print_list(struct element *list) {
    printf("Current list:\n");

    struct element *p = list;

    while (p->next) {
        printf("element: name=%s, addr=%04x\n", p->name, p->address);
        p = p->next;
    }
}

void register_identifier(char *ident, unsigned short addr) {
    // struct element *new_element = malloc(sizeof(struct element));
    // new_element->name = strdup(ident);
    // new_element->address = addr;
    // new_element->next = identifiers;
    // identifiers = new_element;

    // printf("%s=%d\n", ident, zp);

    // print_list(identifiers);

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

%}

%token MNEMONIC
%token ADDRESS
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
%type<addr> ADDRESS
%type<addr> ZEROPAGE

%%

program:
    program expression '\n' { linecounter++; }
|   program '\n'            { linecounter++; }
|	program error '\n'		{ yyerrok; }
|   /* NOTHING */
;

expression:
    MNEMONIC { processMnemonic($1); }
|   MNEMONIC ADDRESS { zp($1, $2); }
|   MNEMONIC '(' ZEROPAGE COMMAX ')' { zpx($1, $3);}
|   IDENTIFIER ADDRESS { register_identifier($1, $2); }
|   DIRECTIVE ADDRESS { directive($1, $2); }
;

%%

