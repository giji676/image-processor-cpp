#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include <cctype>
#include "pixel.h"
#include "bmp.h"
#include "imageProcessor.h"

class Image {
private:
    BMP bmp;
    ImageProcessor imageProcessor;
public:
    std::vector<std::vector<rgbPixel>> rgbImage;
    std::vector<std::vector<grayPixel>> grayImage;
    void open(const std::string& filename);
    void grayscale(const std::vector<std::vector<rgbPixel>>& pixels);
    void gaussian(std::vector<std::vector<grayPixel>>& pixels, int radius);

    template <typename PixelType>
    void save(const std::string& filename, const std::vector<std::vector<PixelType>>& pixels);
};

template <typename PixelType>
void Image::save(const std::string& filename, const std::vector<std::vector<PixelType>>& pixels) {
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
        bmp.saveImage(filename, pixels);

    } else if (end == "png") {
        std::cout << end << " type not yet supported" << std::endl;
    } else if (end == "jpg" || end == "jpeg") {
        std::cout << end << " type not yet supported" << std::endl;
    }
}

#endif // IMAGE_H
