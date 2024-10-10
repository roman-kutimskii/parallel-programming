#ifndef BLUR_H
#define BLUR_H

#include <vector>

#include "../libbmp/libbmp.h"

const int KERNEL_RADIUS = 10; // Радиус ядра
const double SIGMA = 10.0;

// Создание гауссового ядра
std::vector<std::vector<double>> createGaussianKernel(int radius, double sigma);

// Применение гауссового размытия к одному пикселю
RGB applyGaussianBlur(int x, int y, const Image& image, const std::vector<std::vector<double>>& kernel, int width, int height);

// Функция для отражения индекса
int reflect(int index, int max);

#endif //BLUR_H
