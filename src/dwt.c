//  Name:        dwt.h
//  Date:        2026-10-28
//  Author:      Jan Štěpánek <xstepaj00>


#include "dwt.h"


void update(host_buffer_t *self, kernel_buffer_t *kernel_buffer)
{
    if (self->width != kernel_buffer->width) {
        resize(kernel_buffer, -1, 0);
    }

    if (self->height != kernel_buffer->height) {
        resize(kernel_buffer, -1, 1);
    }

    for (cl_uint y = 0; y < self->height; y++) {
        for (cl_uint x = 0; x < self->width; x++) {
            self->data[y * self->width + x] =
                (unsigned char)(128.0f + kernel_buffer->data[y * self->width + x] * 1.5f);
        }
    }
}


void init(kernel_buffer_t *self, const host_buffer_t *host_buffer)
{
    cl_uint width = (cl_uint)host_buffer->width;
    cl_uint height = (cl_uint)host_buffer->height;

    self->data = malloc(width * height * sizeof(cl_short));

    for (cl_uint y = 0; y < height; y++) {
        for (cl_uint x = 0; x < width; x++) {
            self->data[y * width + x] =
                (cl_short)host_buffer->data[y * width + x];
        }
    }

    self->width = width;
    self->height = height;
}

void prepare_for_lifting(kernel_buffer_t *self, cl_uint axis)
{
    if (axis == 0 && self->width % 2 == 0) {
        resize(self, 1, axis);
        mirror(self, axis);
    } else if (axis == 1 && self->height % 2 == 0) {
        resize(self, 1, axis);
        mirror(self, axis);
    }
}


void mirror(
    kernel_buffer_t *buffer,
    const cl_uint axis)
{
    cl_uint vertical = (axis == 1);

    cl_uint width = buffer->width;
    cl_uint height = buffer->height;
    
    cl_short *data = buffer->data;

    if ((!vertical && width < 3) || (vertical && height < 3)) {
        return;
    }

    for (cl_uint i = 0; i < (vertical ? width : height); i++) {
        cl_uint index = vertical ?
            width * (height - 1) + i :
            width * (i + 1) - 1;
        data[index] = data[vertical ? index - 2 * width : index - 2];
    }
}


void resize(
    kernel_buffer_t *buffer,
    const cl_int n,
    const cl_uint axis)
{
    cl_uint vertical = (axis == 1);

    cl_uint width = buffer->width;
    cl_uint height = buffer->height;

    cl_uint new_width = vertical ? width : width + n;
    cl_uint new_height = vertical ? height + n : height;

    cl_short *new_data = malloc(new_width * new_height * sizeof(cl_short));

    for (cl_uint y = 0; y < (n > 0 ? height : new_height); y++) {
        for (cl_uint x = 0; x < (n > 0 ? width : new_width); x++) {
            new_data[y * new_width + x] =
                buffer->data[y * width + x];
        }
    }

    free(buffer->data);

    buffer->data = new_data;
    buffer->width = new_width;
    buffer->height = new_height;
}