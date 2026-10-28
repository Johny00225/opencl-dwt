//  Name:        main.c
//  Date:        2026-10-16
//  Author:      Jan Štěpánek <xstepaj00>


#include "main.h"


int main(int argc, char** argv)
{
    args_t args = {0};
    parse_args(argc, argv, &args);

    BMP* bmp;

    bmp = BMP_ReadFile(args.in_file);
    BMP_CHECK_ERROR(stderr, 1);

    unsigned int width, height;

    width = (unsigned int)BMP_GetWidth(bmp);
    height = (unsigned int)BMP_GetHeight(bmp);

    unsigned char *r_buf = malloc(sizeof(unsigned char) * (int)width * (int)height);
    unsigned char *g_buf = malloc(sizeof(unsigned char) * (int)width * (int)height);
    unsigned char *b_buf = malloc(sizeof(unsigned char) * (int)width * (int)height);

    for (unsigned long x = 0; x < width; x++) {
        for (unsigned long y = 0; y < height; y++) {
            BMP_GetPixelRGB(bmp, x, y, &r_buf[x + y*width], &g_buf[x + y*width], &b_buf[x + y*width]);
        }
    }

    host_buffer_t host_buffer;
    host_buffer.data = r_buf;
    host_buffer.width = width;
    host_buffer.height = height;

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

    kernel_buffer_t kernel_input_buffer;
    init(&kernel_input_buffer, &host_buffer);
    prepare_for_lifting(&kernel_input_buffer, 0);
    prepare_for_lifting(&kernel_input_buffer, 1);

    kernel_buffer_t kernel_output_buffer;
    init(&kernel_output_buffer, &host_buffer);
    prepare_for_lifting(&kernel_output_buffer, 0);
    prepare_for_lifting(&kernel_output_buffer, 1);

    size_t count = (size_t)kernel_input_buffer.width * (size_t)kernel_input_buffer.height;
    size_t bytes = count * sizeof(cl_short);

    cl_mem buffer_input =
        clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            bytes,
            (void*)kernel_input_buffer.data,
            &error
        );

    check(error, "clCreateBuffer(kernel_input_buffer.data)");

    cl_mem buffer_output =
        clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            bytes,
            (void*)kernel_output_buffer.data,
            &error
        );

    check(error, "clCreateBuffer(kernel_output_buffer.data)");


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
            &kernel_input_buffer.width
        ),
        "clSetKernelArg(2)"
    );

    check(
        clSetKernelArg(
            kernel,
            3,
            sizeof(cl_uint),
            &kernel_input_buffer.height
        ),
        "clSetKernelArg(3)"
    );

    cl_uint axis = 0;

    check(
        clSetKernelArg(
            kernel,
            4,
            sizeof(cl_uint),
            &axis
        ),
        "clSetKernelArg(4)"
    );


    // Launch kernel

    size_t global_h[2] = {
        (kernel_input_buffer.width + 1) / 2,
        kernel_input_buffer.height
    };

    check(
        clEnqueueNDRangeKernel(
            queue,
            kernel,
            2,
            NULL,
            global_h,
            NULL,
            0,
            NULL,
            NULL
        ),
        "clEnqueueNDRangeKernel"
    );

    axis = 1;

    size_t global_v[2] = {
        kernel_input_buffer.width,
        (kernel_input_buffer.height + 1) / 2
    };

    check(
        clSetKernelArg(
            kernel,
            0,
            sizeof(cl_mem),
            &buffer_output
        ),
        "clSetKernelArg(0)"
    );

    check(
        clSetKernelArg(
            kernel,
            1,
            sizeof(cl_mem),
            &buffer_input
        ),
        "clSetKernelArg(1)"
    );

    check(
        clSetKernelArg(
            kernel,
            4,
            sizeof(cl_uint),
            &axis
        ),
        "clSetKernelArg(4)"
    );

    check(
        clEnqueueNDRangeKernel(
            queue,
            kernel,
            2,
            NULL,
            global_v,
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
            buffer_input,
            CL_TRUE,
            0,
            bytes,
            kernel_output_buffer.data,
            0,
            NULL,
            NULL
        ),
        "clEnqueueReadBuffer"
    );

    update(&host_buffer, &kernel_output_buffer);

    for (unsigned long x = 0; x < width; x++) {
        for (unsigned long y = 0; y < height; y++) {
            BMP_SetPixelRGB(bmp, x, y, host_buffer.data[x + y*width], 0, 0);
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