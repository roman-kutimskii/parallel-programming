#include <chrono>
#include <iostream>
#include <vector>
#include <windows.h>

#include "blur/blur.h"
#include "libbmp/libbmp.h"

struct ThreadData {
    const Image *inputImage;
    Image *outputImage;
    int kernelSize;
    double sigma;
};

DWORD WINAPI BlurThread(LPVOID lpParam) {
    const auto *data = static_cast<ThreadData *>(lpParam);
    *data->outputImage = applyGaussianBlur(*data->inputImage, data->kernelSize, data->sigma);
    return 0;
};

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <number of threads> <number of cores> <input file> <output file>" <<
                std::endl;
        return 1;
    }

    int numThreads = std::stoi(argv[1]);
    int numCores = std::stoi(argv[2]);
    std::string inputFile = argv[3];
    std::string outputFile = argv[4];

    int width, height;
    Image pixels;

    auto start = std::chrono::high_resolution_clock::now();

    readBMP(inputFile, width, height, pixels);

    Image blurredImage(height, std::vector<RGB>(width));

    std::vector<HANDLE> threads(numThreads);
    std::vector<ThreadData> threadData(numThreads);
    std::vector<Image> sourceImagePieces(numThreads);
    std::vector<Image> blurredImagePieces(numThreads);
    int rowsPerThread = height / numThreads;
    DWORD_PTR affinityMask = (1 << numCores) - 1;

    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;

        sourceImagePieces[i] = Image(pixels.begin() + startRow, pixels.begin() + endRow);
        blurredImagePieces[i] = Image(endRow - startRow, std::vector<RGB>(width));

        threadData[i].inputImage = &sourceImagePieces[i];
        threadData[i].outputImage = &blurredImagePieces[i];
        threadData[i].kernelSize = 50;
        threadData[i].sigma = 100.0;

        threads[i] = CreateThread(nullptr, 0, BlurThread, &threadData[i], CREATE_SUSPENDED, nullptr);

        if (threads[i] == nullptr) {
            std::cerr << "Failed to create thread " << i << std::endl;
            return 1;
        }

        SetThreadAffinityMask(threads[i], affinityMask);
    }

    for (const HANDLE &thread: threads) {
        if (ResumeThread(thread) == static_cast<DWORD>(-1)) {
            std::cerr << "Error: unable to resume thread" << std::endl;
            return 1;
        }
    }

    WaitForMultipleObjects(numThreads, threads.data(), TRUE, INFINITE);

    for (HANDLE thread: threads) {
        CloseHandle(thread);
    }

    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        for (int j = 0; j < blurredImagePieces[i].size(); ++j) {
            blurredImage[startRow + j] = blurredImagePieces[i][j];
        }
    }


    writeBMP(outputFile, width, height, blurredImage);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << numCores << "\t" << numThreads << "\t" << elapsed.count() << std::endl;

    return 0;
}
