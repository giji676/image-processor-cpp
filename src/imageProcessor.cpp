#include "imageProcessor.h"
#include <cmath>
#include <algorithm>

double ImageProcessor::gaussianKernelGenerator(int x, int y, double sigma) {
    return (1 / (2 * M_PI * sigma * sigma)) * std::exp(-(x * x + y * y) / (2 * sigma * sigma));
}

int ImageProcessor::reflectIndex(int index, int max_index) {
    if (index < 0) {
        return -index - 1;
    } else if (index >= max_index) {
        return 2 * max_index - index - 1;
    }
    return index;
}

void ImageProcessor::gaussian(std::vector<std::vector<grayPixel>>& pixels, int radius) {
    double sigma = std::max(radius / 2.0, 1.0);
    int size = radius * 2 + 1;
    std::vector<std::vector<double>> kernel(size, std::vector<double>(size, 0.0));
    double kernelSum = 0.0;

    // Generate Gaussian kernel
    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            kernel[i + radius][j + radius] = gaussianKernelGenerator(j, i, sigma);
            kernelSum += kernel[i + radius][j + radius];
        }
    }

    // Normalize the kernel
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            kernel[i][j] /= kernelSum;
        }
    }

    // Apply Gaussian blur
    std::vector<std::vector<grayPixel>> output = pixels; // Temporary output for safe in-place modification
    for (int y = 0; y < pixels.size(); ++y) {
        for (int x = 0; x < pixels[y].size(); ++x) {
            double sumVal = 0.0;
            for (int i = -radius; i <= radius; ++i) {
                for (int j = -radius; j <= radius; ++j) {
                    int neighbourY = reflectIndex(y + i, pixels.size());
                    int neighbourX = reflectIndex(x + j, pixels[0].size());
                    sumVal += pixels[neighbourY][neighbourX].g * kernel[i + radius][j + radius];
                }
            }
            output[y][x].g = static_cast<int>(std::clamp(sumVal, 0.0, 255.0));
        }
    }

    pixels = output;
}

void ImageProcessor::sobel(const std::vector<std::vector<grayPixel>>& pixels,
                                 std::vector<std::vector<grayPixel>>& outputPixels,
                                 std::vector<std::vector<int>>& gXOut,
                                 std::vector<std::vector<int>>& gYOut) {
    if (pixels.size() != outputPixels.size() || pixels[0].size() != outputPixels[0].size()) {
        outputPixels.resize(pixels.size(), std::vector<grayPixel>(pixels[0].size()));
    }
    if (pixels.size() != gXOut.size() || pixels[0].size() != gXOut[0].size()) {
        gXOut.resize(pixels.size(), std::vector<int>(pixels[0].size()));
    }
    if (pixels.size() != gYOut.size() || pixels[0].size() != gYOut[0].size()) {
        gYOut.resize(pixels.size(), std::vector<int>(pixels[0].size()));
    }

    for (int y = 0; y < pixels.size(); ++y) {
        for (int x = 0; x < pixels[y].size(); ++x) {
            double gXSum = 0;
            double gYSum = 0;
            for (int j = -1; j <= 1; ++j) {
                for (int i = -1; i <= 1; ++i) {
                    int neighbourY = reflectIndex(y + j, pixels.size());
                    int neighbourX = reflectIndex(x + i, pixels[0].size());
                    gXSum += pixels[neighbourY][neighbourX].g * ImageProcessor::Gx[j + 1][i + 1];
                    gYSum += pixels[neighbourY][neighbourX].g * ImageProcessor::Gy[j + 1][i + 1];
                }
            }
            gXOut[y][x] = static_cast<int>(gXSum);
            gYOut[y][x] = static_cast<int>(gYSum);

            double G = sqrt(gXSum * gXSum + gYSum * gYSum);
            G = remap(G, 0, sqrt(255*255 + 255*255), 0, 255);

            outputPixels[y][x].g = static_cast<uint8_t>(G);
        }
    }
}

std::vector<std::vector<grayPixel>> ImageProcessor::grayscale(const std::vector<std::vector<rgbPixel>>& pixels) {
    std::vector<std::vector<grayPixel>> gPixels(pixels.size(), std::vector<grayPixel>(pixels[0].size()));
    for (int i = 0; i < pixels.size(); ++i) {
        for (int j = 0; j < pixels[i].size(); ++j) {
            gPixels[i][j].g = 0.229 * pixels[i][j].r + 0.587 * pixels[i][j].g + 0.114 * pixels[i][j].b;
        }
    }
    return gPixels;
}
