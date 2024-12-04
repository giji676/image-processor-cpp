#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

template <typename T>
T lEndianUInt(std::vector<char> vals, int offset) {
    T newValue = 0;
    int numBytes = sizeof(T);

    for (int i = 0; i < numBytes; ++i) {
        newValue |= (static_cast<uint8_t>(vals[offset + i]) << (i * 8));
    }
    return newValue;
}

struct __attribute__((packed)) fileHeader {
    char signature[2];  // 2 bytes
    uint32_t size;      // 4 bytes
    uint16_t reserve1;  // 2 bytes
    uint16_t reserve2;  // 2 bytes
    uint32_t offset;    // 4 bytes
};

fileHeader extractFileHeader(const std::vector<char>& bytes) {
    return *reinterpret_cast<const fileHeader*>(bytes.data());
}

struct __attribute__((packed)) bitmapInfoHeader {
    uint32_t size;  // 4 u bytes
    int32_t width;  // 4 bytes
    int32_t height;  // 4 bytes
    uint16_t planes;  // 2 u bytes
    uint16_t bitCount;  // 2 u bytes
    uint32_t compression;  // 4 u bytes
    uint32_t sizeImage;  // 4 u bytes
    int32_t horizontalRes;  // 4 bytes
    int32_t verticalRes;  // 4 bytes
    uint32_t colorsUsed;  // 4 u bytes
    uint32_t colorsImportant;  // 4 u bytes
};

bitmapInfoHeader extractBitmapInfoHeader(const std::vector<char>& bytes) {
    return *reinterpret_cast<const bitmapInfoHeader*>(bytes.data());
}

std::vector<char> readFileBytes(std::ifstream& file, int size) {
    std::vector<char> fileBytes(size);
    file.read(fileBytes.data(), fileBytes.size());

    if (file.gcount() < fileBytes.size()) {
        std::cerr << "Error: Could not read enough bytes" << std::endl;
        return {};
    }

    return fileBytes;
}

struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

std::vector<std::vector<pixel>> readPixelData(std::ifstream& file,
                                              int width,
                                              int height,
                                              int row_size) {
    std::vector<std::vector<pixel>> pixel_rows(height, std::vector<pixel>(width));

    for (int i = 0; i < height; ++i) {
        // Read raw row data (width * 3 bytes for RGB)
        std::vector<char> row_data = readFileBytes(file, row_size);

        // Fill pixel_row with RGB data
        for (int j = 0; j < width; ++j) {
            pixel_rows[i][j] = {
                static_cast<uint8_t>(row_data[j * 3 + 2]),
                static_cast<uint8_t>(row_data[j * 3 + 1]),
                static_cast<uint8_t>(row_data[j * 3])
            };
        }
    }
    return pixel_rows;
}

void printFileHeader(fileHeader fileHeader) {
    std::cout << "File Header" << std::endl;
    std::cout << "Signature: " << fileHeader.signature[0] << fileHeader.signature[1] << std::endl;
    std::cout << "Size: " << fileHeader.size << std::endl;
    std::cout << "Reserve1: " << fileHeader.reserve1 << std::endl;
    std::cout << "Reserve2: " << fileHeader.reserve2 << std::endl;
    std::cout << "Offset: " << fileHeader.offset << std::endl;
}

void printBitmapInfoHeader(bitmapInfoHeader bitmapInfoHeader) {
    std::cout << "Bitmap Info Header" << std::endl;
    std::cout << "Size: " << bitmapInfoHeader.size << std::endl;
    std::cout << "Width: " << bitmapInfoHeader.width << std::endl;
    std::cout << "Height: " << bitmapInfoHeader.height << std::endl;
    std::cout << "Planes: " << bitmapInfoHeader.planes << std::endl;
    std::cout << "BitCount: " << bitmapInfoHeader.bitCount << std::endl;
    std::cout << "Compression: " << bitmapInfoHeader.compression << std::endl;
    std::cout << "SizeImage: " << bitmapInfoHeader.sizeImage << std::endl;
    std::cout << "HorizontalRes: " << bitmapInfoHeader.horizontalRes << std::endl;
    std::cout << "VerticalRes: " << bitmapInfoHeader.verticalRes << std::endl;
    std::cout << "ColorsUsed: " << bitmapInfoHeader.colorsUsed << std::endl;
    std::cout << "ColorsImportant: " << bitmapInfoHeader.colorsImportant << std::endl;
}

void printPixels(std::vector<std::vector<pixel>> pixels) {
    for (int i = 0; i < pixels.size(); ++i) {
        for (int j = 0; j < pixels[i].size(); ++j) {
            std::cout << static_cast<int>(pixels[i][j].r)  << "|";
            std::cout << static_cast<int>(pixels[i][j].g)  << "|";
            std::cout << static_cast<int>(pixels[i][j].b)  << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    std::ifstream file("test.bmp", std::ios::binary);

    fileHeader fileHeader;
    std::vector<char> bytes = readFileBytes(file, sizeof(fileHeader));
    fileHeader = extractFileHeader(bytes);

    int bitmapInfoHeaderOffset = 14;
    file.seekg(bitmapInfoHeaderOffset, std::ios::beg);

    bitmapInfoHeader bitmapInfoHeader;
    std::vector<char> bitmapInfoHeaderBytes = readFileBytes(file, sizeof(bitmapInfoHeader));
    bitmapInfoHeader = extractBitmapInfoHeader(bitmapInfoHeaderBytes);

    int row_size = static_cast<int>((bitmapInfoHeader.bitCount *
                                     bitmapInfoHeader.width + 31) / 32) * 4;
    int pixel_array_size = row_size * std::abs(bitmapInfoHeader.height);
    file.seekg(fileHeader.offset, std::ios::beg);

    std::vector<std::vector<pixel>> pixel_rows = readPixelData(file, bitmapInfoHeader.width, bitmapInfoHeader.height, row_size);

    file.close();
    return 0;
}
