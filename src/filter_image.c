#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define PI 3.14159265359

void l1_normalize(image img)
{
    float total = 0;
    for (int i = 0; i < img.c; i++) {
        for (int j = 0; j < img.h; j++) {
            for (int k = 0; k < img.w; k++) {
                total += get_pixel(img, k, j, i);
            }
        }
    }

    for (int i = 0; i < img.c; i++) {
        for (int j = 0; j < img.h; j++) {
            for (int k = 0; k < img.w; k++) {
                set_pixel(img, k, j, i, get_pixel(img, k, j, i) / total);
            }
        }
    }
}

image make_box_filter(int size)
{
    image filter = make_image(size, size, 1);

    for (int j = 0; j < filter.h; j++) {
        for (int i = 0; i < filter.w; i++) {
            set_pixel(filter, i, j, 0, 1.0 / (size * size));
        }
    }

    return filter;
}

image convolve_image(image img, image filter, int preserve)
{
    assert(filter.c == 1 || filter.c == img.c);

    int padding = filter.w / 2;
    int width = img.w + 2 * padding;
    int height = img.h + 2 * padding;
    image padded = make_image(width, height, img.c);

    if (preserve) {
        for (int i = 0; i < img.c; i++) {
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < width; k++) {
                    if (k < padding || k >= width - padding || j < padding || j >= height - padding) {
                        set_pixel(padded, k, j, i, 0);
                    } else {
                        set_pixel(padded, k, j, i, get_pixel(img, k - padding, j - padding, i));
                    }
                }
            }
        }
    } else {
        for (int i = 0; i < img.c; i++) {
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < width; k++) {
                    set_pixel(padded, k, j, i, get_pixel(img, k - padding, j - padding, i));
                }
            }
        }
    }

    image convolved = make_image(img.w, img.h, img.c);

    for (int i = 0; i < img.c; i++) {
        for (int j = 0; j < img.h; j++) {
            for (int k = 0; k < img.w; k++) {
                float sum = 0;
                for (int n = -padding; n <= padding; n++) {
                    for (int m = -padding; m <= padding; m++) {
                        float img_val = get_pixel(padded, k + padding + m, j + padding + n, i);
                        float filter_val = get_pixel(filter, padding + m, padding + n, (filter.c == 1) ? 0 : i);
                        sum += img_val * filter_val;
                    }
                }
                set_pixel(convolved, k, j, i, sum);
            }
        }
    }

    free_image(padded);

    return convolved;
}

image make_highpass_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 5);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);
    return filter;
}

image make_sharpen_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 5);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);
    return filter;
}

image make_emboss_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 2, 2, 0, 2);
    set_pixel(filter, 0, 0, 0, -2);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 1, 2, 0, 1);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 1);
    set_pixel(filter, 2, 1, 0, 1);
    
    return filter;
}

image make_gaussian_filter(float sigma)
{
    int size = (int)ceilf(sigma * 6);
    if (size % 2 == 0) size++;
    image filter = make_image(size, size, 1);

    float total = 0;
    for (int j = 0; j < filter.h; j++) {
        for (int i = 0; i < filter.w; i++) {
            float u = i - (size - 1) / 2;
            float v = j - (size - 1) / 2;
            float value = expf(-(u * u + v * v) / (2 * sigma * sigma));
            set_pixel(filter, i, j, 0, value);
            total += value;
        }
    }

    l1_normalize(filter);

    return filter;
}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image result = make_image(a.w, a.h, a.c);

    for (int i = 0; i < a.c; i++) {
        for (int j = 0; j < a.h; j++) {
            for (int k = 0; k < a.w; k++) {
                float val_a = get_pixel(a, k, j, i);
                float val_b = get_pixel(b, k, j, i);
                set_pixel(result, k, j, i, val_a + val_b);
            }
        }
    }

    return result;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image result = make_image(a.w, a.h, a.c);

    for (int i = 0; i < a.c; i++) {
        for (int j = 0; j < a.h; j++) {
            for (int k = 0; k < a.w; k++) {
                float val_a = get_pixel(a, k, j, i);
                float val_b = get_pixel(b, k, j, i);
                set_pixel(result, k, j, i, val_a - val_b);
            }
        }
    }

    return result;
}

image make_gx_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 1, 0, -2);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 1, 0, 0, 0);
    set_pixel(filter, 1, 2, 0, 0);
    set_pixel(filter, 2, 2, 0, 1);
    set_pixel(filter, 2, 0, 0, 1);
    set_pixel(filter, 0, 2, 0, -1);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 2, 1, 0, 2);
    
    
    return filter;
}

image make_gy_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 1, 0, 0, -2);
    set_pixel(filter, 2, 0, 0, -1);
    set_pixel(filter, 0, 1, 0, 0);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 2, 1, 0, 0);
    set_pixel(filter, 0, 2, 0, 1);
    set_pixel(filter, 1, 2, 0, 2);
    set_pixel(filter, 2, 2, 0, 1);
    return filter;
}

void feature_normalize(image img)
{
    float min_value = INFINITY;
    float max_value = -INFINITY;

    for (int i = 0; i < img.c; i++) {
        for (int j = 0; j < img.h; j++) {
            for (int k = 0; k < img.w; k++) {
                float value = get_pixel(img, k, j, i);
                if (value < min_value) min_value = value;
                if (value > max_value) max_value = value;
            }
        }
    }

    float range = max_value - min_value;

    if (range == 0) return;

    for (int i = 0; i < img.c; i++) {
        for (int j = 0; j < img.h; j++) {
            for (int k = 0; k < img.w; k++) {
                float value = get_pixel(img, k, j, i);
                set_pixel(img, k, j, i, (value - min_value) / range);
            }
        }
    }
}

image *sobel_image(image img)
{
    image *sobel = calloc(2, sizeof(image));

    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();

    sobel[0] = convolve_image(img, gx_filter, 0);
    sobel[1] = convolve_image(img, gy_filter, 0);

    free_image(gx_filter);
    free_image(gy_filter);

    return sobel;
}

image colorize_sobel(image img)
{
    image *sobel = apply_sobel(img);

    image magnitude = make_image(img.w, img.h, 1);
    image angle = make_image(img.w, img.h, 1);

    for (int j = 0; j < img.h; j++) {
        for (int i = 0; i < img.w; i++) {
            float gx = get_pixel(sobel[0], i, j, 0);
            float gy = get_pixel(sobel[1], i, j, 0);
            float mag = sqrtf(gx * gx + gy * gy);
            float theta = atan2f(gy, gx);
            set_pixel(magnitude, i, j, 0, mag);
            set_pixel(angle, i, j, 0, theta);
        }
    }

    feature_normalize(magnitude);

    image colorized = make_image(img.w, img.h, 3);

    for (int j = 0; j < img.h; j++) {
        for (int i = 0; i < img.w; i++) {
            float mag = get_pixel(magnitude, i, j, 0);
            float theta = get_pixel(angle, i, j, 0);
            set_pixel(colorized, i, j, 0, theta / PI);
            set_pixel(colorized, i, j, 1, mag);
            set_pixel(colorized, i, j, 2, mag);
        }
    }

    hsv_to_rgb(colorized);

    free_image(*sobel);
    free_image(*(sobel + 1));
    free(sobel);

    free_image(magnitude);
    free_image(angle);

    return colorized;
}
