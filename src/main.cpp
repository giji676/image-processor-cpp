#include <vector>
#include <cstdint>
#include <string>
#include "bmp.h"
#include "pixel.h"
#include "imageProcessor.h"
#include "image.h"

template <typename T>
T lEndianUInt(std::vector<char> vals, int offset) {
    T newValue = 0;
    int numBytes = sizeof(T);

    for (int i = 0; i < numBytes; ++i) {
        newValue |= (static_cast<uint8_t>(vals[offset + i]) << (i * 8));
    }
    return newValue;
}

int main(int argc, char* argv[]) {
    // expects the following:
    //      ./main, inputFileName, outputFileName, processType, (gaussian ? radius : _)
    // eg. ./main, input.bmp, output.bmp, gaussian, 5
    // eg. ./main, input.bmp, output.bmp, gaussian
    // eg. ./main, input.bmp, output.bmp, sobel

    if (argc < 4) {
        return 0;
    }

    Image image;

    std::string outputFilename = "output.bmp";
    outputFilename = std::string(argv[2]);

    image.open(std::string(argv[1]));

    if (std::string(argv[3]) == "gaussian") {
        int radius = 3;
        if (argc == 5) {
            radius = std::stoi(std::string(argv[4]));
        }
        image.grayscale(image.rgbImage);
        image.gaussian(image.grayImage, radius);
        image.save(outputFilename, image.grayImage);
    } /*else if (std::string(argv[3]) == "sobel") {
        std::vector<std::vector<grayPixel>> sobelPixels;
        std::vector<std::vector<int>> gXOut;
        std::vector<std::vector<int>> gYOut;
        processor.sobel(gPixels, sobelPixels, gXOut, gYOut);
        bmp.saveImage(outputFilename, sobelPixels);
    }*/

    return 0;
}
