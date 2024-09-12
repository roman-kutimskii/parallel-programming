#include <iostream>
#include <syncstream>
#include <vector>
#include <windows.h>

DWORD WINAPI MyThreadFunction(const LPVOID lpParam) {
    auto syncStream = std::osyncstream(std::cout);
    const auto threadNum = *static_cast<int *>(lpParam);
    delete static_cast<int *>(lpParam);
    syncStream << "Thread #" << threadNum << " is running..." << std::endl;
    Sleep(1000);
    syncStream << "Thread #" << threadNum << " is terminating..." << std::endl;
    return 0;
};

int main(const int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads>" << std::endl;
        return 1;
    }

    const int numThreads = std::stoi(argv[1]);
    std::vector<HANDLE> threads(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        const auto threadNum = new int(i + 1);
        threads[i] = CreateThread(nullptr, 0, MyThreadFunction, threadNum, CREATE_SUSPENDED, nullptr);

        if (threads[i] == nullptr) {
            std::cerr << "Error: unable to create thread " << i + 1 << std::endl;
            delete threadNum;
        }
    }

    for (const HANDLE &thread: threads) {
        ResumeThread(thread);
    }


    WaitForMultipleObjects(numThreads, threads.data(), true, INFINITE);

    for (int i = 0; i < numThreads; ++i) {
        CloseHandle(threads[i]);
    }

    return 0;
}
