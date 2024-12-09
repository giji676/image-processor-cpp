#include <iostream>
#include <cctype>
#include <vector>
#include "image.h"
#include "pixel.h"

void Image::open(const std::string& filename) {
    size_t dotPosition = filename.find(".");
    if (dotPosition == std::string::npos) {
        std::cerr << "Error: Invalid filename. No file extension found." << std::endl;
        return;
    }

    std::string start = filename.substr(0, dotPosition);
    std::string end = filename.substr(dotPosition + 1);

    for (char &c:end) {
        c = std::tolower(c);
    }
    if (end == "bmp") {
        Image::rgbImage = Image::bmp.loadBMP(filename);

    } else if (end == "png") {
        std::cout << end << " type not yet supported" << std::endl;
    } else if (end == "jpg" || end == "jpeg") {
        std::cout << end << " type not yet supported" << std::endl;
    }
}

void Image::grayscale(const std::vector<std::vector<rgbPixel>>& pixels) {
    Image::grayImage = Image::imageProcessor.grayscale(pixels);
}

void Image::gaussian(std::vector<std::vector<grayPixel>>& pixels, int radius) {
    Image::imageProcessor.gaussian(pixels, radius);
    Image::grayImage = pixels;
}
