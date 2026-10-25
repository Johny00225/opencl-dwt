inline long floor_div(long n, long divisor)
{
    // divisor must be positive
    long q = n / divisor;
    long r = n % divisor;

    return q - (r < 0);
}

// calculate both approximation and detail
// width must be positive and odd
__kernel void lifting_scheme(
    __global const int *input,
    __global int *output,
    uint width)
{
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);

    size_t approx_width = ((size_t)width + 1) / 2;

    if (width == 0 || (width & 1u) == 0 || x >= approx_width) {
        return;
    }

    size_t row = y * (size_t)width;
    size_t i = x * 2; // x * 2 because processing every even pixel (by width)
    size_t index = row + i;

    if (width == 1) {
        output[row] = input[index];
        return;
    }

    long center = (long)input[index];
    long d_l, d_r;

    // detail
    if (x == 0) {
        // left edge -> mirror right detail
        d_r = (long)input[index + 1]
            - floor_div(center + (long)input[index + 2], 2);
        d_l = d_r;
    } else {
        d_l = (long)input[index - 1]
            - floor_div((long)input[index - 2] + center, 2);
        
        if (x == approx_width - 1) {
            // right edge -> mirror left detail
            d_r = d_l;
        } else {
            d_r = (long)input[index + 1]
                - floor_div(center + input[index + 2], 2);
        }
    }

    // write the right detail coefficient
    // last even position has no right detail
    if (x < approx_width - 1) {
        output[row + approx_width + x] = (int)d_r;
    }

    // approximation    
    output[row + x] = (int)(center + floor_div(d_l + d_r + 2, 4));

    output[index] = 100;
    //printf("done");
}