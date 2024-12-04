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
    // IiiHHIIiiII
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

int main() {
    std::ifstream file("example.bmp", std::ios::binary);

    fileHeader fileHeader;
    std::vector<char> bytes = readFileBytes(file, sizeof(fileHeader));
    fileHeader = extractFileHeader(bytes);

    int bitmapInfoHeaderOffset = 14;
    file.seekg(bitmapInfoHeaderOffset);

    bitmapInfoHeader bitmapInfoHeader;
    std::vector<char> bitmapInfoHeaderBytes = readFileBytes(file, sizeof(bitmapInfoHeader));
    bitmapInfoHeader = extractBitmapInfoHeader(bitmapInfoHeaderBytes);

    file.close();

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

    return 0;
}
