//  Name:        utils_opencl.c
//  Date:        2026-10-23
//  Author:      Jan Štěpánek <xstepaj00>


#include "utils_opencl.h"


void check(cl_int error, const char *operation) {
    if (error != CL_SUCCESS) {
        fprintf(
            stderr,
            "%s failed with OpenCL error %d\n",
            operation,
            error
        );

        exit(EXIT_FAILURE);
    }
}


void find_platforms(cl_uint *platform_count, cl_platform_id **platforms) {
    check(
        clGetPlatformIDs(
            0,
            NULL,
            platform_count
        ),
        "clGetPlatformIDs"
    );

    printf("Platforms found: %u\n", *platform_count);

    if (platform_count == 0) {
        fprintf(stderr, "No OpenCL platforms found.\n");
        exit(EXIT_FAILURE);
    }

    *platforms = malloc(sizeof(cl_platform_id) * *platform_count);

    check(
        clGetPlatformIDs(
            *platform_count,
            *platforms,
            NULL
        ),
        "clGetPlatformIDs"
    );
}


void find_gpu(
    cl_int *error,
    cl_uint platform_count,
    cl_platform_id **platforms,
    cl_device_id *device,
    cl_platform_id *selected_platform
) {
    for (cl_uint i = 0; i < platform_count; ++i) {

        cl_uint device_count = 0;

        *error = clGetDeviceIDs(
            (*platforms)[i],
            CL_DEVICE_TYPE_GPU,
            0,
            NULL,
            &device_count
        );

        if (*error == CL_DEVICE_NOT_FOUND)
            continue;

        check(*error, "clGetDeviceIDs");

        if (device_count > 0) {

            check(
                clGetDeviceIDs(
                    (*platforms)[i],
                    CL_DEVICE_TYPE_GPU,
                    1,
                    device,
                    NULL
                ),
                "clGetDeviceIDs"
            );

            *selected_platform = (*platforms)[i];
            break;
        }
    }

    if (!(*device)) {
        fprintf(stderr, "No OpenCL GPU found.\n");
        free(*platforms);
        exit(EXIT_FAILURE);
    }
}


void print_gpu_info(cl_platform_id selected_platform, cl_device_id device) {
    char platform_name[256];
    char device_name[256];

    check(
        clGetPlatformInfo(
            selected_platform,
            CL_PLATFORM_NAME,
            sizeof(platform_name),
            platform_name,
            NULL
        ),
        "clGetPlatformInfo"
    );

    check(
        clGetDeviceInfo(
            device,
            CL_DEVICE_NAME,
            sizeof(device_name),
            device_name,
            NULL
        ),
        "clGetDeviceInfo"
    );

    printf("Platform: %s\n", platform_name);
    printf("Device:   %s\n", device_name);
}


void build_program(cl_int *error, cl_program *program, cl_device_id *device) {
    *error =
        clBuildProgram(
            *program,
            1,
            device,
            NULL,
            NULL,
            NULL
        );

    if (*error != CL_SUCCESS) {

        size_t log_size = 0;

        clGetProgramBuildInfo(
            *program,
            *device,
            CL_PROGRAM_BUILD_LOG,
            0,
            NULL,
            &log_size
        );

        char *log = malloc(log_size + 1);

        clGetProgramBuildInfo(
            *program,
            *device,
            CL_PROGRAM_BUILD_LOG,
            log_size,
            log,
            NULL
        );

        log[log_size] = '\0';

        fprintf(stderr, "\nOpenCL build error:\n%s\n", log);

        free(log);

        exit(EXIT_FAILURE);
    }
}


cl_int *uchar_to_clint(const long n, const unsigned char *in) {
    cl_int *out = malloc(sizeof(cl_int) * n);
    
    for (int i = 0; i < n; i++) {
        out[i] = (cl_int)in[i];
    }

    return out;
}