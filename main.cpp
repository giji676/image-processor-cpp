#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
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

class BMP {
private:
    // Nested structures
    struct __attribute__((packed)) fileHeader {
        char signature[2];  // 2 bytes
        uint32_t size;      // 4 bytes
        uint16_t reserve1;  // 2 bytes
        uint16_t reserve2;  // 2 bytes
        uint32_t offset;    // 4 bytes
    };

    struct __attribute__((packed)) bitmapInfoHeader {
        uint32_t size;          // 4 bytes
        int32_t width;          // 4 bytes
        int32_t height;         // 4 bytes
        uint16_t planes;        // 2 bytes
        uint16_t bitCount;      // 2 bytes
        uint32_t compression;   // 4 bytes
        uint32_t sizeImage;     // 4 bytes
        int32_t horizontalRes;  // 4 bytes
        int32_t verticalRes;    // 4 bytes
        uint32_t colorsUsed;    // 4 bytes
        uint32_t colorsImportant;  // 4 bytes
    };

    fileHeader header;
    bitmapInfoHeader infoHeader;

    // Helper methods
    fileHeader extractFileHeader(const std::vector<char>& bytes) {
        return *reinterpret_cast<const fileHeader*>(bytes.data());
    }

    bitmapInfoHeader extractBitmapInfoHeader(const std::vector<char>& bytes) {
        return *reinterpret_cast<const bitmapInfoHeader*>(bytes.data());
    }

    bool readFileBytes(std::ifstream& file, char* buffer, int size) {
        file.read(buffer, size);
        return file.gcount() == size;
    }

    std::vector<std::vector<rgbPixel>> readPixelData(std::ifstream& file, int width, int height, int row_size) {
        std::vector<std::vector<rgbPixel>> pixelRows(height, std::vector<rgbPixel>(width));
        for (int i = 0; i < height; ++i) {
            // Read raw row data
            std::vector<char> row_data(row_size);
            readFileBytes(file, row_data.data(), row_size);
            // Fill pixels from RGB data
            for (int j = 0; j < width; ++j) {
                pixelRows[i][j] = {
                    static_cast<uint8_t>(row_data[j * 3 + 2]),  // Red
                    static_cast<uint8_t>(row_data[j * 3 + 1]),  // Green
                    static_cast<uint8_t>(row_data[j * 3])       // Blue
                };
            }
        }
        return pixelRows;
    }

    template <typename PixelType>
    void constructFileHeader(const std::vector<std::vector<PixelType>>& pixels, fileHeader* header) {
        if (header == nullptr) {
            std::cerr << "Error: header is null!" << std::endl;
            return;
        }
        if constexpr (!std::is_same_v<PixelType, rgbPixel> && !std::is_same_v<PixelType, grayPixel>) {
            static_assert(false, "Unsupported pixel type. Only rgbPixel and grayPixel are allowed.");
        }

        header->signature[0] = 'B';
        header->signature[1] = 'M';
        int height = pixels.size();
        int width = pixels[0].size();
        int rawImageSize = width*3;
        rawImageSize = (rawImageSize + (rawImageSize%4))*height;
        int totalFileSize = 14 + 40 + rawImageSize;
        header->size = totalFileSize;
        header->reserve1 = 0;
        header->reserve2 = 0;
        header->offset = 14 + 40;
    }

    template <typename PixelType>
    void constructBitmapInfoHeader(const std::vector<std::vector<PixelType>>& pixels, bitmapInfoHeader* infoHeader) {
        if (infoHeader == nullptr) {
            std::cerr << "Error: infoHeader is null!" << std::endl;
            return;
        }
        if constexpr (!std::is_same_v<PixelType, rgbPixel> && !std::is_same_v<PixelType, grayPixel>) {
            static_assert(false, "Unsupported pixel type. Only rgbPixel and grayPixel are allowed.");
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
        int resX = static_cast<int>(standardDPI * mToInch);
        int resY = static_cast<int>(standardDPI * mToInch);
        infoHeader->horizontalRes = resX;
        infoHeader->verticalRes = resY;
        infoHeader->colorsUsed = 0;
        infoHeader->colorsImportant = 0;
    }

public:
    // Public methods
    std::vector<std::vector<rgbPixel>> loadBMP(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Error: Could not open file");
        }

        // Read file header
        std::vector<char> fileHeaderBytes(sizeof(fileHeader));
        readFileBytes(file, fileHeaderBytes.data(), sizeof(fileHeader));
        header = extractFileHeader(fileHeaderBytes);

        // Read bitmap info header
        file.seekg(14, std::ios::beg); // Bitmap info header starts after 14 bytes
        std::vector<char> infoHeaderBytes(sizeof(bitmapInfoHeader));
        readFileBytes(file, infoHeaderBytes.data(), sizeof(bitmapInfoHeader));
        infoHeader = extractBitmapInfoHeader(infoHeaderBytes);

        // Calculate row size and read pixel data
        int row_size = static_cast<int>((infoHeader.bitCount * infoHeader.width + 31) / 32) * 4;
        file.seekg(header.offset, std::ios::beg);
        auto pixelArray = readPixelData(file, infoHeader.width, infoHeader.height, row_size);

        file.close();
        return pixelArray;
    }

    // Debugging/printing methods
    void printFileHeader() const {
        std::cout << "File Header\n";
        std::cout << "Signature: " << header.signature[0] << header.signature[1] << '\n';
        std::cout << "Size: " << header.size << '\n';
        std::cout << "Reserve1: " << header.reserve1 << '\n';
        std::cout << "Reserve2: " << header.reserve2 << '\n';
        std::cout << "Offset: " << header.offset << '\n';
    }

    void printBitmapInfoHeader() const {
        std::cout << "Bitmap Info Header\n";
        std::cout << "Size: " << infoHeader.size << '\n';
        std::cout << "Width: " << infoHeader.width << '\n';
        std::cout << "Height: " << infoHeader.height << '\n';
        std::cout << "Planes: " << infoHeader.planes << '\n';
        std::cout << "BitCount: " << infoHeader.bitCount << '\n';
        std::cout << "Compression: " << infoHeader.compression << '\n';
        std::cout << "SizeImage: " << infoHeader.sizeImage << '\n';
        std::cout << "HorizontalRes: " << infoHeader.horizontalRes << '\n';
        std::cout << "VerticalRes: " << infoHeader.verticalRes << '\n';
        std::cout << "ColorsUsed: " << infoHeader.colorsUsed << '\n';
        std::cout << "ColorsImportant: " << infoHeader.colorsImportant << '\n';
    }

    void printPixels(const std::vector<std::vector<rgbPixel>>& pixels) const {
        for (int i = 0; i < pixels.size(); ++i) {
            for (int j = 0; j < pixels[i].size(); ++j) {
                std::cout << "(";
                std::cout << static_cast<int>(pixels[i][j].r)  << ",";
                std::cout << static_cast<int>(pixels[i][j].g)  << ",";
                std::cout << static_cast<int>(pixels[i][j].b)  << ") ";
            }
            std::cout << std::endl;
        }
    }

    template <typename PixelType>
    void saveImage(const std::string& filename, const std::vector<std::vector<PixelType>>& pixels) {
        if constexpr (std::is_same_v<PixelType, rgbPixel>) {
            // Handle rgbPixel case
        } else if constexpr (std::is_same_v<PixelType, grayPixel>) {
            // Handle grayPixel case
        } else {
            static_assert(std::is_same_v<PixelType, rgbPixel> || std::is_same_v<PixelType, grayPixel>, 
                          "Unsupported pixel type. Only rgbPixel and grayPixel are allowed.");
        }
        std::ofstream file(filename, std::ios::binary);
        fileHeader header_;
        constructFileHeader(pixels, &header_);
        file.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        bitmapInfoHeader infoHeader_;
        constructBitmapInfoHeader(pixels, &infoHeader_);
        file.write(reinterpret_cast<const char*>(&infoHeader_), sizeof(infoHeader_));
        file.seekp(header_.offset, std::ios::beg);

        int rowWidth = pixels[0].size();
        int padding = (rowWidth*3)%4;
        int rowSize = rowWidth*3 + padding;

        for (int i = 0; i < pixels.size(); ++i) {
            std::vector<char> rowData(rowSize, 0);

            for (int j = 0; j < rowWidth; ++j) {
                if constexpr (std::is_same_v<PixelType, rgbPixel>) {
                    rowData[j*3] = pixels[i][j].b;
                    rowData[j*3 + 1] = pixels[i][j].g;
                    rowData[j*3 + 2] = pixels[i][j].r;
                } else if constexpr (std::is_same_v<PixelType, grayPixel>) {
                    rowData[j*3] = pixels[i][j].g;
                    rowData[j*3 + 1] = pixels[i][j].g;
                    rowData[j*3 + 2] = pixels[i][j].g;
                } else {
                    static_assert(std::is_same_v<PixelType, rgbPixel>
                        || std::is_same_v<PixelType, grayPixel>,
                        "Unsupported pixel type. Only rgbPixel and grayPixel are allowed.");
                }
            }
            file.write(rowData.data(), rowSize);
        }
        file.close();
    }
};

int main(int argc, char* argv[]) {
    if (argc == 1) {
        return 0;
    }

    ImageProcessor processor;
    BMP bmp;
    std::string outputFilename = "output.bmp";
    std::vector<std::vector<rgbPixel>> pixels;
    std::vector<std::vector<grayPixel>> gPixels;
    if (argc >= 2) {
        pixels = bmp.loadBMP(std::string(argv[1]));
    }
    if (argc >= 3) {
        outputFilename = std::string(argv[2]);
    }
    if (argc >= 4) {
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
    }

    return 0;
}
