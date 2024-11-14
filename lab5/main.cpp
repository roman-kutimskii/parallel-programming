#include <windows.h>
#include <string>
#include <fstream>
#include <vector>

HANDLE FileMutex;

int ReadFromFile() {
    std::fstream myfile("balance.txt", std::ios_base::in);
    int result;
    myfile >> result;
    myfile.close();
    return result;
}

void WriteToFile(int data) {
    std::fstream myfile("balance.txt", std::ios_base::out);
    myfile << data << std::endl;
    myfile.close();
}

int GetBalance() {
    int balance = ReadFromFile();
    return balance;
}

void Deposit(int money) {
    WaitForSingleObject(FileMutex, INFINITE);

    int balance = GetBalance();
    balance += money;

    WriteToFile(balance);
    printf("Balance after deposit: %d\n", balance);

    ReleaseMutex(FileMutex);
}

void Withdraw(int money) {
    WaitForSingleObject(FileMutex, INFINITE);

    if (GetBalance() < money) {
        printf("Cannot withdraw money, balance lower than %d\n", money);
        ReleaseMutex(FileMutex);
        return;
    }

    Sleep(20);
    int balance = GetBalance();
    balance -= money;
    WriteToFile(balance);
    printf("Balance after withdraw: %d\n", balance);

    ReleaseMutex(FileMutex);
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

    FileMutex = CreateMutex(nullptr, FALSE, "Global\\FileMutex");

    if (FileMutex == nullptr) {
        printf("CreateMutex error: %d\n", GetLastError());
        return 1;
    }

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

    WaitForMultipleObjects(50, handles.data(), true, INFINITE);
    printf("Final Balance: %d\n", GetBalance());

    getchar();

    CloseHandle(FileMutex);

    return 0;
}
