#ifndef PIXEL_H
#define PIXEL_H

#include <cstdint> // For uint8_t

struct rgbPixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct grayPixel {
    uint8_t g;
};

#endif // PIXEL_H
