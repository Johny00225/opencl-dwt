#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    cl_uint platform_count = 0;

    cl_int err = clGetPlatformIDs(
        0,
        NULL,
        &platform_count
    );

    if (err != CL_SUCCESS) {
        printf("clGetPlatformIDs failed: %d\n", err);
        return 1;
    }

    printf("Platforms: %u\n", platform_count);

    if (platform_count == 0)
        return 0;

    cl_platform_id *platforms =
        malloc(sizeof(cl_platform_id) * platform_count);

    clGetPlatformIDs(
        platform_count,
        platforms,
        NULL
    );

    for (cl_uint i = 0; i < platform_count; ++i) {

        char platform_name[512];

        clGetPlatformInfo(
            platforms[i],
            CL_PLATFORM_NAME,
            sizeof(platform_name),
            platform_name,
            NULL
        );

        printf("Platform: %s\n", platform_name);

        cl_uint device_count = 0;

        clGetDeviceIDs(
            platforms[i],
            CL_DEVICE_TYPE_ALL,
            0,
            NULL,
            &device_count
        );

        cl_device_id *devices =
            malloc(sizeof(cl_device_id) * device_count);

        clGetDeviceIDs(
            platforms[i],
            CL_DEVICE_TYPE_ALL,
            device_count,
            devices,
            NULL
        );

        for (cl_uint j = 0; j < device_count; ++j) {

            char device_name[512];

            clGetDeviceInfo(
                devices[j],
                CL_DEVICE_NAME,
                sizeof(device_name),
                device_name,
                NULL
            );

            printf("  Device: %s\n", device_name);
        }

        free(devices);
    }

    free(platforms);

    return 0;
}