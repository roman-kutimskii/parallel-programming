#include <iostream>
#include <memory>
#include <vector>
#include <windows.h>
#include <timeapi.h>
#include <fstream>
#include <sstream>

DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
    auto threadNumPtr = std::unique_ptr<int>(static_cast<int *>(lpParam));
    const int threadNum = *threadNumPtr;

    std::ostringstream filename;
    filename << "thread_" << threadNum << ".txt";
    std::ofstream outFile(filename.str());
    std::ostringstream output;
    if (!outFile.is_open()) {
        std::cerr << "Error: unable to open file for thread " << threadNum << std::endl;
        return 1;
    }

    for (int i = 0; i < 20; ++i) {
        DWORD currentTime = timeGetTime();
        output << threadNum << "|" << currentTime << std::endl;
        for (int j = 0; j < 1'000'000; ++j) {
            for (int k = 0; k < 1'000; ++k) {
            }
        }
    }

    outFile << output.str();

    outFile.close();
    return 0;
}

int main(const int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads> [priority_thread_number]" << std::endl;
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

    int priorityThreadNum = -1;
    if (argc == 3) {
        try {
            priorityThreadNum = std::stoi(argv[2]);
            if (priorityThreadNum < 1 || priorityThreadNum > numThreads) {
                std::cerr << "Invalid priority thread number: " << argv[2] << std::endl;
                return 1;
            }
        } catch (const std::invalid_argument &e) {
            std::cerr << "Invalid priority thread number: " << argv[2] << std::endl;
            return 1;
        } catch (const std::out_of_range &e) {
            std::cerr << "Priority thread number out of range: " << argv[2] << std::endl;
            return 1;
        }
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

        if (priorityThreadNum == i + 1) {
            if (!SetThreadPriority(threads[i], THREAD_PRIORITY_HIGHEST)) {
                std::cerr << "Error: unable to set thread priority for thread " << i + 1 << std::endl;
            }
        }
    }

    std::cout << "Press Enter to continue..." << std::endl;
    std::cin.get();

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
