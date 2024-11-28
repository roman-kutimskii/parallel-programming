#include <chrono>
#include <iostream>

double calculatePiSync(const long numIterations) {
    double pi = 0;
    for (long i = 0; i < numIterations; i++) {
        if (i % 2 == 0) {
            pi += 1.0 / (2 * i + 1);
        } else {
            pi -= 1.0 / (2 * i + 1);
        }
    }
    return pi * 4.0;
}

double calculatePiParallelWrong(const long numIterations) {
    double pi = 0;
#pragma omp parallel for default(none) shared(pi) shared(numIterations)
    for (long i = 0; i < numIterations; i++) {
        if (i % 2 == 0) {
            pi += 1.0 / (2 * i + 1);
        } else {
            pi -= 1.0 / (2 * i + 1);
        }
    }
    return pi * 4.0;
}

double calculatePiParallelAtomic(const long numIterations) {
    double pi = 0;
#pragma omp parallel for default(none) shared(pi) shared(numIterations)
    for (long i = 0; i < numIterations; i++) {
        const double term = (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
#pragma omp atomic
        pi += term;
    }
    return pi * 4.0;
}

double calculatePiParallelReduction(const long numIterations) {
    double pi = 0;
#pragma omp parallel for default(none) shared(numIterations) reduction(+:pi)
    for (long i = 0; i < numIterations; i++) {
        const double term = (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
        pi += term;
    }
    return pi * 4.0;
}

int main() {
    constexpr long numIterations = 1'000'000'000;

    // Занимает больше времени, так как выполняется последовательно.
    // Approximate value of PI (sync): 3.14159, Time: 0.920924 seconds
    auto startTime = std::chrono::high_resolution_clock::now();
    double pi = calculatePiSync(numIterations);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;
    std::cout << "Approximate value of PI (sync): " << pi << ", Time: " << duration.count() << " seconds" << std::endl;

    // Быстрое выполнение, но некорректный результат.
    // Approximate value of PI (parallel wrong): 1.66667e-09, Time: 0.124428 seconds
    startTime = std::chrono::high_resolution_clock::now();
    pi = calculatePiParallelWrong(numIterations);
    endTime = std::chrono::high_resolution_clock::now();
    duration = endTime - startTime;
    std::cout << "Approximate value of PI (parallel wrong): " << pi << ", Time: " << duration.count() << " seconds" <<
            std::endl;

    // Корректный результат, но с накладными расходами на синхронизацию.
    // Approximate value of PI (parallel atomic): 3.14159, Time: 74.673 seconds
    startTime = std::chrono::high_resolution_clock::now();
    pi = calculatePiParallelAtomic(numIterations);
    endTime = std::chrono::high_resolution_clock::now();
    duration = endTime - startTime;
    std::cout << "Approximate value of PI (parallel atomic): " << pi << ", Time: " << duration.count() << " seconds" <<
            std::endl;

    // Баланс между скоростью и корректностью
    // Approximate value of PI (parallel reduction): 3.14159, Time: 0.134691 seconds
    startTime = std::chrono::high_resolution_clock::now();
    pi = calculatePiParallelReduction(numIterations);
    endTime = std::chrono::high_resolution_clock::now();
    duration = endTime - startTime;
    std::cout << "Approximate value of PI (parallel reduction): " << pi << ", Time: " << duration.count() << " seconds"
            << std::endl;

    return 0;
}
