#ifndef LIBBMP_H
#define LIBBMP_H

#include <cstdint>
#include <string>
#include <vector>

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

void readBMP(const std::string &filename, int32_t &width, int32_t &height, Image &pixels);

void writeBMP(const std::string &filename, int32_t width, int32_t height, const Image &pixels);

#endif // LIBBMP_H
