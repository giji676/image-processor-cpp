#ifndef BMP_H
#define BMP_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include "pixel.h"

class BMP {
private:
    struct __attribute__((packed)) fileHeader {
        char signature[2];
        uint32_t size;
        uint16_t reserve1;
        uint16_t reserve2;
        uint32_t offset;
    };

    struct __attribute__((packed)) bitmapInfoHeader {
        uint32_t size;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bitCount;
        uint32_t compression;
        uint32_t sizeImage;
        int32_t horizontalRes;
        int32_t verticalRes;
        uint32_t colorsUsed;
        uint32_t colorsImportant;
    };

    fileHeader header;
    bitmapInfoHeader infoHeader;

    fileHeader extractFileHeader(const std::vector<char>& bytes);
    bitmapInfoHeader extractBitmapInfoHeader(const std::vector<char>& bytes);
    bool readFileBytes(std::ifstream& file, char* buffer, int size);
    std::vector<std::vector<rgbPixel>> readPixelData(std::ifstream& file, int width, int height, int row_size);

    template <typename PixelType>
    void constructFileHeader(const std::vector<std::vector<PixelType>>& pixels, fileHeader* header);

    template <typename PixelType>
    void constructBitmapInfoHeader(const std::vector<std::vector<PixelType>>& pixels, bitmapInfoHeader* infoHeader);

public:
    std::vector<std::vector<rgbPixel>> loadBMP(const std::string& filename);
    void printFileHeader() const;
    void printBitmapInfoHeader() const;
    void printPixels(const std::vector<std::vector<rgbPixel>>& pixels) const;
    
    template <typename PixelType>
    void saveImage(const std::string& filename, const std::vector<std::vector<PixelType>>& pixels);

};

// Function template definition
template <typename PixelType>
void BMP::saveImage(const std::string& filename, const std::vector<std::vector<PixelType>>& pixels) {
    static_assert(std::is_same_v<PixelType, rgbPixel> || std::is_same_v<PixelType, grayPixel>, 
                  "Unsupported pixel type. Only rgbPixel and grayPixel are allowed.");

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error: Cannot open file for writing");
    }

    fileHeader header_;
    constructFileHeader(pixels, &header_);
    file.write(reinterpret_cast<const char*>(&header_), sizeof(header_));

    bitmapInfoHeader infoHeader_;
    constructBitmapInfoHeader(pixels, &infoHeader_);
    file.write(reinterpret_cast<const char*>(&infoHeader_), sizeof(infoHeader_));

    file.seekp(header_.offset, std::ios::beg);

    int rowWidth = pixels[0].size();
    int padding = (rowWidth * 3) % 4;
    int rowSize = rowWidth * 3 + padding;

    for (int i = 0; i < pixels.size(); ++i) {
        std::vector<char> rowData(rowSize, 0);

        for (int j = 0; j < rowWidth; ++j) {
            if constexpr (std::is_same_v<PixelType, rgbPixel>) {
                rowData[j * 3] = pixels[i][j].b;
                rowData[j * 3 + 1] = pixels[i][j].g;
                rowData[j * 3 + 2] = pixels[i][j].r;
            } else if constexpr (std::is_same_v<PixelType, grayPixel>) {
                rowData[j * 3] = pixels[i][j].g;
                rowData[j * 3 + 1] = pixels[i][j].g;
                rowData[j * 3 + 2] = pixels[i][j].g;
            }
        }
        file.write(rowData.data(), rowSize);
    }

    file.close();
}

template <typename PixelType>
void BMP::constructFileHeader(const std::vector<std::vector<PixelType>>& pixels, fileHeader* header) {
    if (header == nullptr) {
        std::cerr << "Error: header is null!" << std::endl;
        return;
    }

    header->signature[0] = 'B';
    header->signature[1] = 'M';

    int height = pixels.size();
    int width = pixels[0].size();
    int rawImageSize = width * 3;
    rawImageSize = (rawImageSize + (rawImageSize % 4)) * height;

    header->size = 14 + 40 + rawImageSize;
    header->reserve1 = 0;
    header->reserve2 = 0;
    header->offset = 14 + 40;
}

template <typename PixelType>
void BMP::constructBitmapInfoHeader(const std::vector<std::vector<PixelType>>& pixels, bitmapInfoHeader* infoHeader) {
    if (infoHeader == nullptr) {
        std::cerr << "Error: infoHeader is null!" << std::endl;
        return;
    }

    infoHeader->size = 40;

    int height = pixels.size();
    int width = pixels[0].size();
    int rawImageSize = width * 3;
    rawImageSize = (rawImageSize + (rawImageSize % 4)) * height;

    infoHeader->width = width;
    infoHeader->height = height;
    infoHeader->planes = 1;
    infoHeader->bitCount = 24;
    infoHeader->compression = 0;
    infoHeader->sizeImage = rawImageSize;

    int standardDPI = 96;
    double mToInch = 39.3701;
    infoHeader->horizontalRes = static_cast<int>(standardDPI * mToInch);
    infoHeader->verticalRes = static_cast<int>(standardDPI * mToInch);
    infoHeader->colorsUsed = 0;
    infoHeader->colorsImportant = 0;
}
#endif // BMP_H
