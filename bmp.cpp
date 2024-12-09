#include "bmp.h"


BMP::fileHeader BMP::extractFileHeader(const std::vector<char>& bytes) {
    return *reinterpret_cast<const fileHeader*>(bytes.data());
}

BMP::bitmapInfoHeader BMP::extractBitmapInfoHeader(const std::vector<char>& bytes) {
    return *reinterpret_cast<const bitmapInfoHeader*>(bytes.data());
}

bool BMP::readFileBytes(std::ifstream& file, char* buffer, int size) {
    file.read(buffer, size);
    return file.gcount() == size;
}

std::vector<std::vector<rgbPixel>> BMP::readPixelData(std::ifstream& file, int width, int height, int row_size) {
    std::vector<std::vector<rgbPixel>> pixelRows(height, std::vector<rgbPixel>(width));

    for (int i = 0; i < height; ++i) {
        std::vector<char> row_data(row_size);
        readFileBytes(file, row_data.data(), row_size);

        for (int j = 0; j < width; ++j) {
            pixelRows[i][j] = {
                static_cast<uint8_t>(row_data[j * 3 + 2]), // Red
                static_cast<uint8_t>(row_data[j * 3 + 1]), // Green
                static_cast<uint8_t>(row_data[j * 3])      // Blue
            };
        }
    }
    return pixelRows;
}

std::vector<std::vector<rgbPixel>> BMP::loadBMP(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file");
    }

    std::vector<char> fileHeaderBytes(sizeof(fileHeader));
    readFileBytes(file, fileHeaderBytes.data(), sizeof(fileHeader));
    header = extractFileHeader(fileHeaderBytes);

    file.seekg(14, std::ios::beg);
    std::vector<char> infoHeaderBytes(sizeof(bitmapInfoHeader));
    readFileBytes(file, infoHeaderBytes.data(), sizeof(bitmapInfoHeader));
    infoHeader = extractBitmapInfoHeader(infoHeaderBytes);

    int row_size = static_cast<int>((infoHeader.bitCount * infoHeader.width + 31) / 32) * 4;
    file.seekg(header.offset, std::ios::beg);

    auto pixelArray = readPixelData(file, infoHeader.width, infoHeader.height, row_size);
    file.close();

    return pixelArray;
}

void BMP::printFileHeader() const {
    std::cout << "File Header\n";
    std::cout << "Signature: " << header.signature[0] << header.signature[1] << '\n';
    std::cout << "Size: " << header.size << '\n';
    std::cout << "Reserve1: " << header.reserve1 << '\n';
    std::cout << "Reserve2: " << header.reserve2 << '\n';
    std::cout << "Offset: " << header.offset << '\n';
}

void BMP::printBitmapInfoHeader() const {
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

void BMP::printPixels(const std::vector<std::vector<rgbPixel>>& pixels) const {
    for (const auto& row : pixels) {
        for (const auto& pixel : row) {
            std::cout << "(" << static_cast<int>(pixel.r) << ","
                      << static_cast<int>(pixel.g) << ","
                      << static_cast<int>(pixel.b) << ") ";
        }
        std::cout << '\n';
    }
}

