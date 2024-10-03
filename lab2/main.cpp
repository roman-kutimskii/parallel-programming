#include <chrono>
#include <iostream>
#include <vector>

#include "blur/blur.h"
#include "libbmp/libbmp.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << "<number of threads> <number of cores> <input file> <output file>" <<
                std::endl;
        return 1;
    }

    int width, height;
    Image pixels;

    auto start = std::chrono::high_resolution_clock::now();

    readBMP(argv[3], width, height, pixels);
    Image blurredImage = applyGaussianBlur(pixels, 20, 100);
    writeBMP(argv[4], width, height, blurredImage);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
