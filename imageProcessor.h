#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <vector>
#include "pixel.h" // Ensure this contains definitions for grayPixel and rgbPixel.

class ImageProcessor {

private:
    int Gx[3][3] = {
        {-1,  0,  1},
        {-2,  0,  2},
        {-1,  0,  1}
    };

    int Gy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };
    double gaussianKernelGenerator(int x, int y, double sigma);
    int reflectIndex(int index, int max_index);

    double remap(double val, double inMin, double inMax, double outMin, double outMax) {
        return std::max(outMin, std::min((val - inMin) * (outMax - outMin) / (inMax - inMin) + outMin, outMax));
    }

public:
    void gaussian(std::vector<std::vector<grayPixel>>& pixels, int radius);
    void sobel(const std::vector<std::vector<grayPixel>>& pixels,
        std::vector<std::vector<grayPixel>>& outputPixels,
        std::vector<std::vector<int>>& gXOut,
        std::vector<std::vector<int>>& gYOut);
    std::vector<std::vector<grayPixel>> grayscale(const std::vector<std::vector<rgbPixel>>& pixels);
};

#endif // IMAGEPROCESSOR_H
