#include <vector>
#include <cstdint>
#include <string>
#include "bmp.h"
#include "pixel.h"
#include "imageProcessor.h"

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

    ImageProcessor processor;
    BMP bmp;
    std::string outputFilename = "output.bmp";
    std::vector<std::vector<rgbPixel>> pixels;
    std::vector<std::vector<grayPixel>> gPixels;

    pixels = bmp.loadBMP(std::string(argv[1]));
    outputFilename = std::string(argv[2]);

    gPixels = processor.grayscale(pixels);
    if (std::string(argv[3]) == "gaussian") {
        int radius = 3;
        if (argc == 5) {
            radius = std::stoi(std::string(argv[4]));
        }
        processor.gaussian(gPixels, radius);
        bmp.saveImage(outputFilename, gPixels);
    } else if (std::string(argv[3]) == "sobel") {
        std::vector<std::vector<grayPixel>> sobelPixels;
        std::vector<std::vector<int>> gXOut;
        std::vector<std::vector<int>> gYOut;
        processor.sobel(gPixels, sobelPixels, gXOut, gYOut);
        bmp.saveImage(outputFilename, sobelPixels);
    }

    return 0;
}
