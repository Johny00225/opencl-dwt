//  Name:        main.c
//  Date:        2026-10-16
//  Author:      Jan Štěpánek <xstepaj00>


#include "main.h"


int main(int argc, char** argv)
{
    args_t args = {0};
    parse_args(argc, argv, &args);

    BMP* bmp;
    unsigned long width, height;

    bmp = BMP_ReadFile(args.in_file);
    BMP_CHECK_ERROR(stderr, 1);

    width = BMP_GetWidth(bmp);
    height = BMP_GetHeight(bmp);

    unsigned char *r_buf = malloc(sizeof(unsigned char) * (int)width * (int)height);
    unsigned char *g_buf = malloc(sizeof(unsigned char) * (int)width * (int)height);
    unsigned char *b_buf = malloc(sizeof(unsigned char) * (int)width * (int)height);

    for (unsigned long x = 0; x < width; x++) {
        for (unsigned long y = 0; y < height; y++) {
            BMP_GetPixelRGB(bmp, x, y, &r_buf[x + y*width], &g_buf[x + y*width], &b_buf[x + y*width]);
        }
    }


    // OpenCL init

    cl_int error;

    cl_uint platform_count = 0;
    cl_platform_id *platforms;

    find_platforms(&platform_count, &platforms);


    cl_device_id device = NULL;
    cl_platform_id selected_platform = NULL;

    find_gpu(
        &error,
        platform_count,
        &platforms,
        &device,
        &selected_platform
    );


    print_gpu_info(selected_platform, device);


    cl_context context =
        clCreateContext(
            NULL,
            1,
            &device,
            NULL,
            NULL,
            &error
        );

    check(error, "clCreateContext");


    cl_command_queue queue =
        clCreateCommandQueue(
            context,
            device,
            0,
            &error
        );

    check(error, "clCreateCommandQueue");


    // Create kernel input buffers

    long n = (int)width * (int)height;

    cl_uint width_cl = 2593;
    size_t height_cl = 1944;

    cl_int *input = uchar_to_clint(n, mirror_right((int)width, (int)height, r_buf));
    cl_int *output = malloc(sizeof(cl_int) * (n + height));

    size_t count = (size_t)width * height;
    size_t bytes = count * sizeof(cl_int);

    cl_mem buffer_input =
        clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            bytes,
            (void*)input,
            &error
        );

    check(error, "clCreateBuffer(r_buf)");

    cl_mem buffer_output =
        clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            bytes,
            (void*)output,
            &error
        );

    check(error, "clCreateBuffer(output)");

    
    // Create kernel

    char *source =
        read_file("kernels/lifting_scheme.cl");

    const char *sources[] = {
        source
    };


    cl_program program =
        clCreateProgramWithSource(
            context,
            1,
            sources,
            NULL,
            &error
        );

    check(error, "clCreateProgramWithSource");


    build_program(&error, &program, &device);


    cl_kernel kernel =
        clCreateKernel(
            program,
            "lifting_scheme",
            &error
        );

    check(error, "clCreateKernel");


    // Set kernel args

    check(
        clSetKernelArg(
            kernel,
            0,
            sizeof(cl_mem),
            &buffer_input
        ),
        "clSetKernelArg(0)"
    );

    check(
        clSetKernelArg(
            kernel,
            1,
            sizeof(cl_mem),
            &buffer_output
        ),
        "clSetKernelArg(1)"
    );

    check(
        clSetKernelArg(
            kernel,
            2,
            sizeof(cl_uint),
            &width_cl
        ),
        "clSetKernelArg(2)"
    );


    // Launch kernel

    size_t global_size[2] = {
        (width_cl + 1) / 2,
        height_cl
    };

    check(
        clEnqueueNDRangeKernel(
            queue,
            kernel,
            2,
            NULL,
            global_size,
            NULL,
            0,
            NULL,
            NULL
        ),
        "clEnqueueNDRangeKernel"
    );




    // Copy results

    check(
        clEnqueueReadBuffer(
            queue,
            buffer_output,
            CL_TRUE,
            0,
            bytes,
            output,
            0,
            NULL,
            NULL
        ),
        "clEnqueueReadBuffer"
    );




    for (unsigned long x = 0; x < width; x++) {
        for (unsigned long y = 0; y < height; y++) {
            BMP_SetPixelRGB(bmp, x, y, output[x + y*width], 0, 0);
        }
    }

    BMP_WriteFile(bmp, args.out_file);
    BMP_CHECK_ERROR(stderr, 1);

    for (unsigned long x = 0; x < width; x++) {
        for (unsigned long y = 0; y < height; y++) {
            BMP_SetPixelRGB(bmp, x, y, r_buf[x + y*width], 0, 0);
        }
    }

    BMP_WriteFile(bmp, "out2.bmp");
    BMP_CHECK_ERROR(stderr, 1);

    BMP_Free( bmp );

    free(r_buf);
    free(g_buf);
    free(b_buf);


    // Cleanup

    clReleaseKernel(kernel);
    clReleaseProgram(program);

    clReleaseMemObject(buffer_input);
    clReleaseMemObject(buffer_output);

    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(source);
    free(platforms);

    return EXIT_SUCCESS;
}