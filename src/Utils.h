#ifndef UTILS_H
#define UTILS_H

#define cimg_display 0
#include "CImg.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

using namespace cimg_library;

// Clamp value to range
inline int clampv(int val, int minv, int maxv) {
    return std::max(minv, std::min(val, maxv));
}

// Safe pixel access with border handling
inline int getp(const CImg<unsigned char>& img, int x, int y, int c) {
    int w = img.width(), h = img.height();
    x = clampv(x, 0, w - 1);
    y = clampv(y, 0, h - 1);
    return img(x, y, c);
}

#endif
