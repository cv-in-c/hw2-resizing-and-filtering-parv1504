#include <math.h>
#include "image.h"

float nn_interpolate(image img, float x, float y, int channel)
{
    int ix = (int)roundf(x);
    int iy = (int)roundf(y);

    return get_pixel(img, ix, iy, channel);
}

image nn_resize(image img, int new_width, int new_height)
{
    image resized = make_image(new_width, new_height, img.c);

    float width_scale = (float)img.w / new_width;
    float height_scale = (float)img.h / new_height;

    for (int k = 0; k < img.c; ++k) {
        for (int j = 0; j < new_height; ++j) {
            for (int i = 0; i < new_width; ++i) {
                float x = (i + 0.5) * width_scale - 0.5;
                float y = (j + 0.5) * height_scale - 0.5;

                float value = nearest_neighbor_interpolate(img, x, y, k);
                set_pixel(resized, i, j, k, value);
            }
        }
    }

    return resized;
}

float bilinear_interpolate(image img, float x, float y, int channel)
{
    int x1 = (int)floorf(x);
    int x2 = x1 + 1;
    int y1 = (int)floorf(y);
    int y2 = y1 + 1;

    float a1 = (x2 - x) * (y2 - y);
    float a2 = (x - x1) * (y2 - y);
    float a3 = (x2 - x) * (y - y1);
    float a4 = (x - x1) * (y - y1);

    float b1 = get_pixel(img, x1, y1, channel);
    float b2 = get_pixel(img, x2, y1, channel);
    float b3 = get_pixel(img, x1, y2, channel);
    float b4 = get_pixel(img, x2, y2, channel);

    return a1 * b1 + a2 * b2 + a3 * b3 + a4 * b4;
}

image bilinear_resize(image img, int new_width, int new_height)
{
    image ans = make_image(new_width, new_height, img.c);

    float width_scale = (float)img.w / new_width;
    float height_scale = (float)img.h / new_height;

    for (int k = 0; k < img.c; ++k) {
        for (int j = 0; j < new_height; ++j) {
            for (int i = 0; i < new_width; ++i) {
                float x = (i + 0.5) * width_scale - 0.5;
                float y = (j + 0.5) * height_scale - 0.5;

                float value = bilinear_interpolate(img, x, y, k);
                set_pixel(ans, i, j, k, value);
            }
        }
    }

    return ans;
}
