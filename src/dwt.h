//  Name:        dwt.h
//  Date:        2026-10-28
//  Author:      Jan Štěpánek <xstepaj00>


#ifndef _DWT_HEADER
#define _DWT_HEADER


#include "utils.h"
#include "utils_opencl.h"


typedef struct {
    unsigned char *data;
    unsigned int width;
    unsigned int height;

    //void (*update)(struct host_buffer_t *, const struct kernel_buffer_t *);
} host_buffer_t;


typedef struct {
    cl_short *data;
    cl_uint width;
    cl_uint height;

    //void (*init)(struct kernel_buffer_t *self, const struct host_buffer_t *host_buffer);
    //void (*prepare_for_lifting)(struct kernel_buffer_t *self, cl_uint axis);
} kernel_buffer_t;


void update(host_buffer_t *self, kernel_buffer_t *kernel_buffer);

void init(kernel_buffer_t *self, const host_buffer_t *host_buffer);

// if width/height is even, extend and mirror
void prepare_for_lifting(kernel_buffer_t *self, cl_uint axis);


void mirror(
    kernel_buffer_t *buffer,
    const cl_uint axis
);

void resize(
    kernel_buffer_t *buffer,
    const cl_int n,
    const cl_uint axis
);


#endif