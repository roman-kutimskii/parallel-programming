#include <chrono>
#include <cmath>
#include <iostream>
#include <windows.h>

#include "blur.h"
#include "bmp.h"

struct ThreadData {
    int startRow;
    int endRow;
    int width;
    int height;
    const Image *image;
    Image *output;
    const std::vector<std::vector<double> > *kernel;
    int threadId;
    std::vector<std::pair<int, long long> > *timingData;
};

DWORD WINAPI ThreadProc(LPVOID lpParameter) {
    auto *data = static_cast<ThreadData *>(lpParameter);
    int startRow = data->startRow;
    int endRow = data->endRow;
    int width = data->width;
    int height = data->height;
    const Image *image = data->image;
    Image *output = data->output;
    const std::vector<std::vector<double> > *kernel = data->kernel;
    int threadId = data->threadId;
    auto *timingData = data->timingData;

    for (int y = startRow; y < endRow; ++y) {
        for (int x = 0; x < width; ++x) {
            auto pixelStart = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 200'000; ++i) {
                (*output)[y][x] = applyGaussianBlur(x, y, *image, *kernel, width, height);
            }
            auto pixelEnd = std::chrono::high_resolution_clock::now();
            long long timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(pixelEnd - pixelStart).
                    count();
            timingData->emplace_back(threadId, timeElapsed);
        }
    }

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
    Image image;

    auto start = std::chrono::high_resolution_clock::now();

    readBMP(inputFile, width, height, image);

    Image output = image;

    std::vector<HANDLE> threads(numThreads);
    std::vector<ThreadData> threadData(numThreads);
    int rowsPerThread = height / numThreads;

    auto kernel = createGaussianKernel(KERNEL_RADIUS, SIGMA);

    std::vector<std::pair<int, long long> > timingData;

    for (int i = 0; i < numThreads; ++i) {
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;
        threadData[i].width = width;
        threadData[i].height = height;
        threadData[i].image = &image;
        threadData[i].output = &output;
        threadData[i].kernel = &kernel;
        threadData[i].threadId = i;
        threadData[i].timingData = &timingData;

        threads[i] = CreateThread(nullptr, 0, ThreadProc, &threadData[i], CREATE_SUSPENDED, nullptr);

        if (threads[i] == nullptr) {
            std::cerr << "Failed to create thread " << i << std::endl;
            return 1;
        }

        DWORD_PTR affinityMask = (1 << i % numCores);
        SetThreadAffinityMask(threads[i], affinityMask);

        int priority;
        switch (i) {
            case 0:
                priority = THREAD_PRIORITY_ABOVE_NORMAL;
                break;
            case 1:
                priority = THREAD_PRIORITY_NORMAL;
                break;
            case 2:
                priority = THREAD_PRIORITY_BELOW_NORMAL;
                break;
            default:
                priority = THREAD_PRIORITY_NORMAL;
                break;
        }
        if (!SetThreadPriority(threads[i], priority)) {
            std::cerr << "Failed to set thread priority for thread " << i << std::endl;
            return 1;
        }
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

    writeBMP(outputFile, width, height, output);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << numCores << "\t" << numThreads << "\t" << elapsed.count() << std::endl;

    std::sort(timingData.begin(), timingData.end());
    std::ofstream logFile("timing.log");
    for (const auto &timing: timingData) {
        logFile << timing.first << "\t" << timing.second << std::endl;
    }

    return 0;
}
