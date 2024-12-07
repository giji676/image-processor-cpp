#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <vector>
#include "pixel.h" // Ensure this contains definitions for grayPixel and rgbPixel.

class ImageProcessor {
private:
    double gaussianKernelGenerator(int x, int y, double sigma);
    int reflectIndex(int index, int max_index);

public:
    void gaussian(std::vector<std::vector<grayPixel>>& pixels, int radius);
    std::vector<std::vector<grayPixel>> grayscale(const std::vector<std::vector<rgbPixel>>& pixels);
};

#endif // IMAGEPROCESSOR_H
