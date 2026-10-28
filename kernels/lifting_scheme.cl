inline int floor_div(int n, int divisor)
{
    // divisor must be positive
    int q = n / divisor;
    int r = n % divisor;

    return q - (r < 0);
}

// calculate both approximation and detail
// width and height must be positive and odd
// axis != 1 -> left to right
// axis == 1 -> top to bottom
__kernel void lifting_scheme(
    __global const short *input,
    __global short *output,
    uint width,
    uint height,
    uint axis)
{
    size_t x = get_global_id(0);
    size_t y = get_global_id(1);

    int vertical = (axis == 1);

    size_t line = vertical ? x : y;
    size_t item = vertical ? y : x;

    size_t line_count = vertical ? width : height;
    size_t item_count = vertical ? height : width;

    size_t approx_item_count = (item_count + 1) / 2;

    // process only half of pixels (every even pixel)
    if (item_count == 0 || (item_count & 1u) == 0 ||
        line >= line_count || item >= approx_item_count) {
        printf("fuck");
        return;
    }

    size_t stride = vertical ? width : 1; // direction of offset/neighbors

    size_t base = vertical ? line : line * width; // line[0]
    size_t offset = 2 * item * stride; // every even pixel of line

    size_t index = base + offset;

    int center = (int)input[index];

    if (item_count == 1) {
        output[base] = (short)center;
        return;
    }

    int d_l, d_r; // detail for neighbors of center

    // detail
    if (item == 0) {
        // left/top edge -> mirror right/bottom detail
        d_r = (int)input[index + stride]
            - floor_div(center + (int)input[index + 2 * stride], 2);
        d_l = d_r;
    } else {
        d_l = (int)input[index - stride]
            - floor_div((int)input[index - 2 * stride] + center, 2);

        if (item == approx_item_count - 1) {
            // right/bottom edge -> mirror left/top detail
            d_r = d_l;
        } else {
            d_r = (int)input[index + stride]
                - floor_div(center + input[index + 2 * stride], 2);
        }
    }

    // approximation
    int a = center + floor_div(d_l + d_r + 2, 4);

    // map approximation coefficients to left/top half of output
    output[base + item * stride] = (short)a;

    // map detail coefficients to right/bottom half of output
    // last even position has no right/bottom detail
    if (item < approx_item_count - 1) {
        output[base + (approx_item_count + item) * stride] = (short)d_r;
    }

    //output[index] = 100;
    //printf("done");
}