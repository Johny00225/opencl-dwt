//  Name:        utils.h
//  Date:        2026-10-21
//  Author:      Jan Štěpánek <xstepaj00>


#ifndef _UTILS_HEADER
#define _UTILS_HEADER


#define CL_TARGET_OPENCL_VERSION 120

#include "QDBMP/qdbmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_NAME_LENGTH 256
#define N 1024


typedef struct {
    char in_file[MAX_FILE_NAME_LENGTH];
    char out_file[MAX_FILE_NAME_LENGTH];
} args_t;


void print_help(char *program);
void parse_args(int argc, char **argv, args_t *args);


char *read_file(const char *filename);


#endif