//  Name:        utils.c
//  Date:        2026-10-21
//  Author:      Jan Štěpánek <xstepaj00>


#include "utils.h"


void print_help(char *program) {
    fprintf(
        stdout,
        "Usage: %s [-h | --help] | [-i <input file> -o <output file>]\n",
        program
    );
    exit(EXIT_SUCCESS);
}


void parse_args(int argc, char **argv, args_t *args) {
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


char *read_file(const char *filename) {
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


void split(const long n, const char *in, char **out_even, char **out_odd) {
    if (n%2 == 0) {
        *out_even = malloc(sizeof(char) * (n/2));
        *out_odd = malloc(sizeof(char) * (n/2));
    } else {
        *out_even = malloc(sizeof(char) * (n/2 + 1));
        *out_odd = malloc(sizeof(char) * (n/2));
    }
    for (int i = 0; i < n; i++) {
        if (i % 2 == 0) {
            (*out_even)[i/2] = in[i];
        } else {
            (*out_odd)[i/2] = in[i];
        }
    }
    for (int i = 0; i < n/2; i++) {
        (*out_even)[i] = in[i*2];
        (*out_odd)[i] = in[i*2 + 1];
    }
    if (n%2 != 0) {
        (*out_even)[n/2] = in[n-1];
    }
}


unsigned char *mirror_right(const int width, const int height, const unsigned char *in) {
    if (width % 2 == 1) return NULL;
    
    unsigned char *out = malloc(sizeof(char) * (width + 1) * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            out[y * (width + 1) + x] = in[y * width + x];
        }
        out[(y + 1) * (width + 1)] = in[(y + 1) * width - 2];
    }

    return out;
}
