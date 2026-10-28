//  Name:        utils.c
//  Date:        2026-10-21
//  Author:      Jan Štěpánek <xstepaj00>


#include "utils.h"


void print_help(char *program)
{
    fprintf(
        stdout,
        "Usage: %s [-h | --help] | [-i <input file> -o <output file>]\n",
        program
    );
    exit(EXIT_SUCCESS);
}


void parse_args(int argc, char **argv, args_t *args)
{
    if (argc > 5) {
        exit(EXIT_FAILURE);
    }

    if (argc < 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        print_help(argv[0]);
    }

    if (!strcmp(argv[1], "-i") && argc >= 3) {
        if (strlen(argv[2]) >= MAX_FILE_NAME_LENGTH) {
            exit(EXIT_FAILURE);
        }
        strcpy(args->in_file, argv[2]);
    }

    if (!strcmp(argv[3], "-o") && argc >= 5) {
        if (strlen(argv[4]) >= MAX_FILE_NAME_LENGTH) {
            exit(EXIT_FAILURE);
        }
        strcpy(args->out_file, argv[4]);
    }
}


char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (!file) {
        fprintf(stderr, "Cannot open %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *data = malloc((size_t)size + 1);

    if (!data) {
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fread(data, 1, (size_t)size, file);
    data[size] = '\0';

    fclose(file);

    return data;
}
