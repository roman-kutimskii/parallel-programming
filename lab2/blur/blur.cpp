#include "blur.h"

Image applyGaussianBlur(const Image &input, int kernelSize, double sigma) {
    int height = input.size();
    int width = input[0].size();
    Image output = input;

    // Создание ядра Гаусса
    std::vector<std::vector<double> > kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0.0;
    int halfSize = kernelSize / 2;
    double sigma2 = 2 * sigma * sigma;

    for (int i = -halfSize; i < halfSize; ++i) {
        for (int j = -halfSize; j < halfSize; ++j) {
            kernel[i + halfSize][j + halfSize] = std::exp(-(i * i + j * j) / sigma2) / (M_PI * sigma2);
            sum += kernel[i + halfSize][j + halfSize];
        }
    }

    // Проверка суммы
    if (sum == 0.0) {
        sum = 1.0;
    }

    // Нормализация ядра
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            kernel[i][j] /= sum;
        }
    }

    // Применение размытия по Гауссу с отражением краев
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double blue = 0.0, green = 0.0, red = 0.0;
            for (int i = -halfSize; i < halfSize; ++i) {
                for (int j = -halfSize; j < halfSize; ++j) {
                    int yy = std::min(std::max(y + i, 0), height - 1);
                    int xx = std::min(std::max(x + j, 0), width - 1);
                    blue += input[yy][xx].blue * kernel[i + halfSize][j + halfSize];
                    green += input[yy][xx].green * kernel[i + halfSize][j + halfSize];
                    red += input[yy][xx].red * kernel[i + halfSize][j + halfSize];
                }
            }
            output[y][x].blue = static_cast<uint8_t>(blue);
            output[y][x].green = static_cast<uint8_t>(green);
            output[y][x].red = static_cast<uint8_t>(red);
        }
    }

    return output;
}
