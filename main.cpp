#include <iostream>
#include <vector>
#include <windows.h>

DWORD WINAPI MyThreadFunction(const LPVOID lpParam) {
    const auto threadNum = *static_cast<int *>(lpParam);
    delete static_cast<int *>(lpParam);
    std::cout << "Thread #" << threadNum << " is running..." << std::endl;
    Sleep(1000);
    std::cout << "Thread #" << threadNum << " is terminating..." << std::endl;
    ExitThread(0);
};

int main(const int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads>" << std::endl;
        return 1;
    }

    const int numThreads = std::stoi(argv[1]);
    std::vector<HANDLE> threads(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        auto threadNum = new int(i + 1);
        threads[i] = CreateThread(nullptr, 0, MyThreadFunction, threadNum, CREATE_SUSPENDED, nullptr);

        if (threads[i] == nullptr) {
            std::cerr << "Error: unable to create thread " << i + 1 << std::endl;
            delete threadNum;
            return 1;
        }
    }

    for (const HANDLE &thread: threads) {
        ResumeThread(thread);
    }

    WaitForMultipleObjects(numThreads, threads.data(), true, INFINITE);

    return 0;
}
