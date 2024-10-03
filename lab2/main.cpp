#include <iostream>
#include <vector>

#include "libbmp/libbmp.h"


int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << "<number of threads> <number of cores> <input file> <output file>" <<
                std::endl;
        return 1;
    }

    int width, height;
    std::vector<RGB> pixels;

    readBMP(argv[3], width, height, pixels);
    writeBMP(argv[4], width, height, pixels);

    return 0;
}
