//  Name:        utils_opencl.h
//  Date:        2026-10-23
//  Author:      Jan Štěpánek <xstepaj00>


#ifndef _UTILS_OPENCL_HEADER
#define _UTILS_OPENCL_HEADER


#define CL_TARGET_OPENCL_VERSION 120

#include "CL/opencl.h"

#include <stdio.h>
#include <stdlib.h>


void check(cl_int error, const char *operation);
void find_platforms(cl_uint *platform_count, cl_platform_id **platforms);
void find_gpu(
    cl_int *error,
    cl_uint platform_count,
    cl_platform_id **platforms,
    cl_device_id *device,
    cl_platform_id *selected_platform
);
void print_gpu_info(cl_platform_id selected_platform, cl_device_id device);
void build_program(cl_int *error, cl_program *program, cl_device_id *device);

cl_int *uchar_to_clint(const long n, const unsigned char *in);

#endif