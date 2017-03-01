#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

char *concat(const char *left, const char *right) {
    size_t len = strlen(left) + strlen(right);
    char *result = malloc(len + 1);
    if (result == NULL) {
        return NULL;
    }
    strcpy(result, left);
    strcat(result, right);
    result[len] = '\0';
    return result;
}

// Assumes 0 <= max <= RAND_MAX
// Returns in the closed interval [0, max]
long random_at_most(long max) {
    unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
            num_bins = (unsigned long) max + 1,
            num_rand = (unsigned long) RAND_MAX + 1,
            bin_size = num_rand / num_bins,
            defect = num_rand % num_bins;

    long x;
    do {
        x = random();
    }
        // This is carefully written not to overflow
    while (num_rand - defect <= (unsigned long) x);

    // Truncated division is intentional
    return x / bin_size;
}

struct hsv_color *rgb_to_hsv(struct rgb_color *rgb) {
    float max = fmaxf(rgb->red, fmaxf(rgb->green, rgb->blue));
    float min = fminf(rgb->red, fminf(rgb->green, rgb->blue));

    float delta = max - min;

    struct hsv_color *hsv = malloc(sizeof(struct hsv_color));
    if (hsv == NULL) {
        return NULL;
    }
    hsv->hue = 0;
    hsv->saturation = 0;
    hsv->value = 0;

    if (min == max) {
        hsv->hue = 0;
    } else if (max == rgb->red) {

        hsv->hue = 60 * (fmodf(rgb->green - rgb->blue, 6) / delta);
    } else if (max == rgb->green) {
        hsv->hue = 60 * ((rgb->blue - rgb->red) / delta + 2);
    } else if (max == rgb->blue) {
        hsv->hue = 60 * ((rgb->red - rgb->green) / delta + 4);
    }

    if (max > 0) {
        hsv->saturation = delta / max;
    }

    if (hsv->hue < 0) {
        hsv->hue += 360;
    }

    return hsv;
};

struct rgb_color *hsv_to_rgb(struct hsv_color *hsv) {

    int hue = ((int)hsv->hue % 360) / 60;

    float c = hsv->value * hsv->saturation;
    float x = c * (1 - abs((hue % 2) - 1));
    float m = hsv->value - c;

    float r = 0;
    float g = 0;
    float b = 0;

    switch (hue) {
        case 0:
            r = c;
            g = x;
            b = 0;
            break;
        case 1:
            r = x;
            g = c;
            b = 0;
            break;
        case 2:
            r = 0;
            g = c;
            b = x;
            break;
        case 3:
            r = 0;
            g = x;
            b = c;
            break;
        case 4:
            r = x;
            g = 0;
            b = c;
            break;
        case 5:
            r = c;
            g = 0;
            b = x;
            break;
        default:
            // HSV to RGB conversion failed!
            return NULL;
    }

    struct rgb_color *rgb = malloc(sizeof(struct rgb_color));
    if (rgb == NULL) {
        return NULL;
    }
    rgb->red = r + m;
    rgb->green = g + m;
    rgb->blue = b + m;

    return rgb;
};

float map_range(float s_min, float s_max, float d_min, float d_max, float value) {
    // s_min <= value <= s_max
    float source_fraction = (s_max - s_min) / (value - s_min);
    float dest_delta = d_max - d_min;
    return d_min + dest_delta * source_fraction;
    // d_min <= result <= d_max
}