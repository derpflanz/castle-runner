#include <argp.h>
#include <stdbool.h>

const char *argp_program_version = "1.0";
static char doc[] = "AsmNG - a re-written CastleRunner assembler";
static char args_doc[] = "-o OUTPUT -i INPUT";
static struct argp_option options[] = {
    {"output", 'o', "FILE", 0, "Write to FILE instead of standard output" },
    {"input", 'i', "FILE", 0, "Read from FILE instead of standard input" },
    {"add_vectors", 'a', NULL, OPTION_ARG_OPTIONAL, "Add RESB and IRQ vectors to hex file"},
    {"show_result", 's', NULL, OPTION_ARG_OPTIONAL, "Show resulting bytes"},
    { 0 }
};

struct arguments {
    char *args[2];
    char *output_file;
    char *input_file;
    int add_vectors;
    int show_result;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch(key) {
        case 'o':
            arguments->output_file = arg;
            break;
        case 'i':
            arguments->input_file = arg;
            break;
        case 'a':
            arguments->add_vectors = 1;
            break;
        case 's':
            arguments->show_result = 1;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

bool arguments_parse(int argc, char **argv, struct arguments *arguments) {
    arguments->output_file = NULL;
    arguments->input_file = NULL;

    argp_parse(&argp, argc, argv, 0, 0, arguments);

    if (arguments->output_file == NULL) {
        printf("You must provide an output file (-o, --output).\n");
        return false;
    }

    if (arguments->input_file == NULL) {
        printf("You must provide an input file (-i, --input).\n");
        return false;
    }

    return true;
}
