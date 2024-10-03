#include <iostream>
#include <memory>
#include <syncstream>
#include <vector>
#include <windows.h>

DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
    auto syncStream = std::osyncstream(std::cout);
    auto threadNumPtr = std::unique_ptr<int>(static_cast<int *>(lpParam));
    const int threadNum = *threadNumPtr;
    syncStream << "Thread #" << threadNum << " is running..." << std::endl;
    Sleep(1000);
    syncStream << "Thread #" << threadNum << " is terminating..." << std::endl;
    return 0;
}

int main(const int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads>" << std::endl;
        return 1;
    }

    int numThreads;
    try {
        numThreads = std::stoi(argv[1]);
    } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid number of threads: " << argv[1] << std::endl;
        return 1;
    } catch (const std::out_of_range &e) {
        std::cerr << "Number of threads out of range: " << argv[1] << std::endl;
        return 1;
    }

    if (numThreads <= 0) {
        std::cerr << "Number of threads must be positive" << std::endl;
        return 1;
    }

    std::vector<HANDLE> threads(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        auto threadNum = std::make_unique<int>(i + 1);
        threads[i] = CreateThread(nullptr, 0, MyThreadFunction, threadNum.release(), CREATE_SUSPENDED, nullptr);

        if (threads[i] == nullptr) {
            std::cerr << "Error: unable to create thread " << i + 1 << std::endl;
            threadNum.reset();
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

    for (const HANDLE &thread: threads) {
        CloseHandle(thread);
    }

    return 0;
}
