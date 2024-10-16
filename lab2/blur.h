#ifndef BLUR_H
#define BLUR_H

#include <vector>

#include "bmp.h"

const int KERNEL_RADIUS = 10; // Радиус ядра
const double SIGMA = 10.0;

std::vector<std::vector<double> > createGaussianKernel(int radius, double sigma) {
    int size = 2 * radius + 1;
    std::vector<std::vector<double> > kernel(size, std::vector<double>(size));
    double sum = 0.0;

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            double exponent = -(x * x + y * y) / (2 * sigma * sigma);
            kernel[y + radius][x + radius] = std::exp(exponent) / (2 * M_PI * sigma * sigma);
            sum += kernel[y + radius][x + radius];
        }
    }

    if (sum == 0.0) {
        sum = 1.0;
    }

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            kernel[y][x] /= sum;
        }
    }

    return kernel;
}

int reflect(int index, int max) {
    if (index < 0) return -index;
    if (index >= max) return 2 * max - index - 1;
    return index;
}

RGB applyGaussianBlur(int x, int y, const Image &image, const std::vector<std::vector<double> > &kernel, int width,
                      int height) {
    double red = 0, green = 0, blue = 0;
    int radius = KERNEL_RADIUS;

    for (int ky = -radius; ky <= radius; ++ky) {
        for (int kx = -radius; kx <= radius; ++kx) {
            int ny = reflect(y + ky, height);
            int nx = reflect(x + kx, width);
            const RGB &pixel = image[ny][nx];
            double weight = kernel[ky + radius][kx + radius];
            red += pixel.red * weight;
            green += pixel.green * weight;
            blue += pixel.blue * weight;
        }
    }

    RGB result;
    result.red = static_cast<uint8_t>(red);
    result.green = static_cast<uint8_t>(green);
    result.blue = static_cast<uint8_t>(blue);
    return result;
}

#endif //BLUR_H
