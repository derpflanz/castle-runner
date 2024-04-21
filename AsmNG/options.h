#include <argp.h>

const char *argp_program_version = "1.0";
static char doc[] = "AsmNG - a re-written CastleRunner assembler";
static char args_doc[] = "-o OUTPUT -i INPUT";
static struct argp_option options[] = {
    {"output", 'o', "FILE", 0, "Write to FILE instead of standard output" },
    {"input", 'i', "FILE", 0, "Read from FILE instead of standard input" },
    { 0 }
};

struct arguments {
    char *args[2];
    char *output_file;
    char *input_file;
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
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

struct arguments arguments_parse(int argc, char **argv) {
    struct arguments arguments;    

    arguments.output_file = NULL;
    arguments.input_file = NULL;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    return arguments;
}