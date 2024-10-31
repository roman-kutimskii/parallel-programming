#ifndef BMP_H
#define BMP_H

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t fileType{0x4D42}; // 'BM'
    uint32_t fileSize;
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offsetData{54};
};

struct BMPInfoHeader {
    uint32_t size{40};
    int32_t width;
    int32_t height;
    uint16_t planes{1};
    uint16_t bitCount{24};
    uint32_t compression{0};
    uint32_t imageSize;
    int32_t xPixelsPerMeter{0};
    int32_t yPixelsPerMeter{0};
    uint32_t colorsUsed{0};
    uint32_t colorsImportant{0};
};
#pragma pack(pop)

struct RGB {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

using Image = std::vector<std::vector<RGB> >;

void readBMP(const std::string &filename, int32_t &width, int32_t &height, Image &pixels) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    file.read(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char *>(&infoHeader), sizeof(infoHeader));

    if (fileHeader.fileType != 0x4D42) {
        std::cerr << "Invalid file format: " << filename << std::endl;
        return;
    }

    width = infoHeader.width;
    height = infoHeader.height;
    pixels.resize(height, std::vector<RGB>(width));

    file.seekg(fileHeader.offsetData, std::ios::beg);

    int32_t rowStride = width * sizeof(RGB);
    int32_t padding = (4 - (rowStride % 4)) % 4;

    for (int32_t y = 0; y < height; ++y) {
        file.read(reinterpret_cast<char *>(pixels[y].data()), rowStride);
        file.ignore(padding); // Пропускаем выравнивание
    }
}

void writeBMP(const std::string &filename, int32_t width, int32_t height, const Image &pixels) {
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    int32_t rowStride = width * sizeof(RGB);
    int32_t padding = (4 - (rowStride % 4)) % 4;
    int32_t paddedRowSize = rowStride + padding;

    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + paddedRowSize * height;
    infoHeader.width = width;
    infoHeader.height = height;
    infoHeader.imageSize = paddedRowSize * height;

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char *>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<const char *>(&infoHeader), sizeof(infoHeader));

    for (const auto &row: pixels) {
        file.write(reinterpret_cast<const char *>(row.data()), rowStride);
        file.write("\0\0\0", padding);
    }
}

#endif //BMP_H
