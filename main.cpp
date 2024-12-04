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

struct fileHeader {
    char signature[2];  // 2 bytes
    uint32_t size;  // 4 bytes
    uint16_t reserve1;  // 2 bytes
    uint16_t reserve2;  // 2 bytes
    uint32_t offset;  // 4 bytes
};

fileHeader extractFileHeader(std::vector<char> bytes) {
    fileHeader fileHeader;
    int offset = 0;
    fileHeader.signature[0] = bytes[0];
    fileHeader.signature[1] = bytes[1];
    offset += sizeof(fileHeader.signature);

    fileHeader.size = lEndianUInt<uint32_t>(bytes, offset);
    offset += sizeof(fileHeader.size);
    fileHeader.reserve1 = lEndianUInt<uint16_t>(bytes, offset);
    offset += sizeof(fileHeader.reserve1);
    fileHeader.reserve2 = lEndianUInt<uint16_t>(bytes, offset);
    offset += sizeof(fileHeader.reserve2);
    fileHeader.offset = lEndianUInt<uint32_t>(bytes, offset);
    offset += sizeof(fileHeader.offset);

    return fileHeader;
}

struct bitmapInfoHeader {
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

bitmapInfoHeader extractBitmapInfoHeader(std::vector<char> bytes) {
    bitmapInfoHeader header;
    int offset = 0;
    header.size = lEndianUInt<uint32_t>(bytes, offset);
    offset += sizeof(header.size);
    header.width = lEndianUInt<int32_t>(bytes, offset);
    offset += sizeof(header.width);
    header.height = lEndianUInt<int32_t>(bytes, offset);
    offset += sizeof(header.height);
    header.planes = lEndianUInt<uint16_t>(bytes, offset);
    offset += sizeof(header.planes);
    header.bitCount = lEndianUInt<uint16_t>(bytes, offset);
    offset += sizeof(header.bitCount);
    header.compression = lEndianUInt<uint32_t>(bytes, offset);
    offset += sizeof(header.compression);
    header.sizeImage = lEndianUInt<uint32_t>(bytes, offset);
    offset += sizeof(header.sizeImage);
    header.horizontalRes = lEndianUInt<int32_t>(bytes, offset);
    offset += sizeof(header.horizontalRes);
    header.verticalRes = lEndianUInt<int32_t>(bytes, offset);
    offset += sizeof(header.verticalRes);
    header.colorsUsed = lEndianUInt<uint32_t>(bytes, offset);
    offset += sizeof(header.colorsUsed);
    header.colorsImportant = lEndianUInt<uint32_t>(bytes, offset);
    offset += sizeof(header.colorsImportant);

    return header;
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
    /*
    std::cout << "File Header" << std::endl;
    std::cout << "Signature: " << fileHeader.signature[0] << fileHeader.signature[1] << std::endl;
    std::cout << "Size: " << fileHeader.size << std::endl;
    std::cout << "Reserve1: " << fileHeader.reserve1 << std::endl;
    std::cout << "Reserve2: " << fileHeader.reserve2 << std::endl;
    std::cout << "Offset: " << fileHeader.offset << std::endl;

    std::cout << std::endl;

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
    */

    int row_size = static_cast<int>((bitmapInfoHeader.bitCount *
                                     bitmapInfoHeader.width + 31) / 32) * 4;
    int pixel_array_size = row_size * std::abs(bitmapInfoHeader.height);
    file.seekg(fileHeader.offset, std::ios::beg);

    std::vector<std::vector<pixel>> pixel_rows = readPixelData(file, bitmapInfoHeader.width, bitmapInfoHeader.height, row_size);
    /*
    for (int i = 0; i < pixel_rows.size(); ++i) {
        for (int j = 0; j < pixel_rows[i].size(); ++j) {
            std::cout << static_cast<int>(pixel_rows[i][j].r)  << "|";
            std::cout << static_cast<int>(pixel_rows[i][j].g)  << "|";
            std::cout << static_cast<int>(pixel_rows[i][j].b)  << " ";
        }
        std::cout << std::endl;
    }
    */

    file.close();
    return 0;
}
