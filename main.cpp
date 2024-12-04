#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

struct fileHeader {
    char signature[2];  // 2 bytes
    uint32_t size;  // 4 bytes
    uint16_t reserve1;  // 2 bytes
    uint16_t reserve2;  // 2 bytes
    uint32_t offset;  // 4 bytes
};

template <typename T>
T lEndianUInt(std::vector<char> vals, int offset) {
    T newValue = 0;
    int numBytes = sizeof(T);

    for (int i = 0; i < numBytes; ++i) {
        newValue |= (static_cast<uint8_t>(vals[offset + i]) << (i * 8));
    }
    return newValue;
}

fileHeader extractFileHeader(std::vector<char> headerBytes) {
    fileHeader header;
    int offset = 0;
    header.signature[0] = headerBytes[0];
    header.signature[1] = headerBytes[1];
    offset += sizeof(header.signature);

    header.size = lEndianUInt<uint32_t>(headerBytes, offset);
    offset += sizeof(header.size);
    header.reserve1 = lEndianUInt<uint16_t>(headerBytes, offset);
    offset += sizeof(header.reserve1);
    header.reserve2 = lEndianUInt<uint16_t>(headerBytes, offset);
    offset += sizeof(header.reserve2);
    header.offset = lEndianUInt<uint32_t>(headerBytes, offset);
    offset += sizeof(header.offset);

    return header;
}

std::vector<char> readFileHeaderBytes(std::ifstream& file, int size) {
    std::vector<char> headerBytes(size);
    file.read(headerBytes.data(), headerBytes.size());

    if (file.gcount() < headerBytes.size()) {
        std::cerr << "Error: Could not read enough bytes" << std::endl;
        return {};  // Return an empty vector on failure
    }

    return headerBytes;
}

int main() {
    std::ifstream file("example.bmp", std::ios::binary);

    fileHeader header;

    std::vector<char> headerBytes = readFileHeaderBytes(file, sizeof(header));
    header = extractFileHeader(headerBytes);

    file.close();

    std::cout << "Signature: " << header.signature[0] << header.signature[1] << std::endl;
    std::cout << "Size: " << header.size << std::endl;
    std::cout << "Reserve1: " << header.reserve1 << std::endl;
    std::cout << "Reserve2: " << header.reserve2 << std::endl;
    std::cout << "Offset: " << header.offset << std::endl;

    return 0;
}
