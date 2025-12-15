#ifndef UTILS_H
#define UTILS_H

// CImg Configuration
#define cimg_display 0
#include "CImg.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>

using namespace cimg_library;

// Clamp value to range
inline int clampv(int val, int minv, int maxv) {
    return std::max(minv, std::min(val, maxv));
}

// Safe pixel access with border handling (Zero padding)
inline int getp(const CImg<unsigned char>& img, int x, int y, int c = 0) {
    if (x < 0 || x >= img.width() || y < 0 || y >= img.height()) return 0;
    return img(x, y, c);
}

// Simple binarization helper
inline CImg<unsigned char> binarize(const CImg<unsigned char>& src, int threshold = 128) {
    CImg<unsigned char> bin = src.get_channel(0);
    cimg_forXY(bin, x, y) {
        bin(x, y) = (bin(x, y) > threshold) ? 255 : 0;
    }
    return bin;
}

#endif
