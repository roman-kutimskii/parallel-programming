#include "libbmp.h"

void readBMP(const std::string &filename, int32_t &width, int32_t &height, std::vector<RGB> &pixels) {
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
    pixels.resize(width * height);

    file.seekg(fileHeader.offsetData, std::ios::beg);
    file.read(reinterpret_cast<char *>(pixels.data()), pixels.size() * sizeof(RGB));
}

void writeBMP(const std::string &filename, int32_t width, int32_t height, const std::vector<RGB> &pixels) {
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + pixels.size() * sizeof(RGB);
    infoHeader.width = width;
    infoHeader.height = height;
    infoHeader.imageSize = pixels.size() * sizeof(RGB);

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char *>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<const char *>(&infoHeader), sizeof(infoHeader));
    file.write(reinterpret_cast<const char *>(pixels.data()), pixels.size() * sizeof(RGB));
}
