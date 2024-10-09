#include "libbmp.h"

void readBMP(const std::string &filename, int32_t &width, int32_t &height, std::vector<std::vector<RGB> > &pixels) {
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

void writeBMP(const std::string &filename, int32_t width, int32_t height,
              const std::vector<std::vector<RGB> > &pixels) {
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
