#ifndef __UTIL
#define __UTIL

char *concat(const char *left, const char *right);
long random_at_most(long max);

struct rgb_color {
    float red;
    float green;
    float blue;
};

struct hsv_color {
    float hue;
    float saturation;
    float value;
};

struct hsv_color* rgb_to_hsv(struct rgb_color* rgb);
struct rgb_color* hsv_to_rgb(struct hsv_color* hsv);

float map_range(float s_min, float s_max, float d_min, float d_max, float value);

#endif