#ifndef BLUR_H
#define BLUR_H

#include <cmath>

#include "../libbmp/libbmp.h"

Image applyGaussianBlur(const Image &input, int kernelSize, double sigma);

#endif //BLUR_H
