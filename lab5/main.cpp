#include <windows.h>
#include <string>
#include <fstream>
#include <vector>

CRITICAL_SECTION FileLockingCriticalSection;

int ReadFromFile() {
    EnterCriticalSection(&FileLockingCriticalSection);
    std::fstream myfile("balance.txt", std::ios_base::in);
    int result;
    myfile >> result;
    myfile.close();
    LeaveCriticalSection(&FileLockingCriticalSection);

    return result;
}

void WriteToFile(int data) {
    EnterCriticalSection(&FileLockingCriticalSection);
    std::fstream myfile("balance.txt", std::ios_base::out);
    myfile << data << std::endl;
    myfile.close();
    LeaveCriticalSection(&FileLockingCriticalSection);
}

int GetBalance() {
    int balance = ReadFromFile();
    return balance;
}

void Deposit(int money) {
    EnterCriticalSection(&FileLockingCriticalSection);

    int balance = GetBalance();
    balance += money;

    WriteToFile(balance);
    printf("Balance after deposit: %d\n", balance);

    LeaveCriticalSection(&FileLockingCriticalSection);
}

void Withdraw(int money) {
    EnterCriticalSection(&FileLockingCriticalSection);

    if (GetBalance() < money) {
        printf("Cannot withdraw money, balance lower than %d\n", money);
        LeaveCriticalSection(&FileLockingCriticalSection);
        return;
    }

    Sleep(20);
    int balance = GetBalance();
    balance -= money;
    WriteToFile(balance);
    printf("Balance after withdraw: %d\n", balance);

    LeaveCriticalSection(&FileLockingCriticalSection);
}

DWORD WINAPI DoDeposit(LPVOID lpParameter) {
    Deposit(static_cast<int>(reinterpret_cast<size_t>(lpParameter)));
    ExitThread(0);
}

DWORD WINAPI DoWithdraw(LPVOID lpParameter) {
    Withdraw(static_cast<int>(reinterpret_cast<size_t>(lpParameter)));
    ExitThread(0);
}

int main() {
    std::vector<HANDLE> handles(50);

    InitializeCriticalSection(&FileLockingCriticalSection);

    WriteToFile(0);

    SetProcessAffinityMask(GetCurrentProcess(), 1);
    for (int i = 0; i < 50; i++) {
        handles[i] = i % 2 == 0
                         ? CreateThread(nullptr, 0, &DoDeposit, reinterpret_cast<LPVOID>(230), CREATE_SUSPENDED,
                                        nullptr)
                         : CreateThread(nullptr, 0, &DoWithdraw, reinterpret_cast<LPVOID>(1000), CREATE_SUSPENDED,
                                        nullptr);
        ResumeThread(handles[i]);
    }


    // ожидание окончания работы двух потоков
    WaitForMultipleObjects(50, handles.data(), true, INFINITE);
    printf("Final Balance: %d\n", GetBalance());

    getchar();

    DeleteCriticalSection(&FileLockingCriticalSection);

    return 0;
}
