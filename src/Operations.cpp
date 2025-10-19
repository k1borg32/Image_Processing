#include "Operations.h"

using namespace cimg_library;
using namespace std;

CImg<unsigned char> op_brightness(const CImg<unsigned char>& src, int value) {
    CImg<unsigned char> out = src;
    cimg_forXY(out, x, y) {
        for (int c = 0; c < out.spectrum(); ++c) {
            int v = out(x, y, c) + value;
            out(x, y, c) = (unsigned char)clampv(v, 0, 255);
        }
    }
    return out;
}

CImg<unsigned char> op_contrast_linear(const CImg<unsigned char>& src, float factor) {
    CImg<unsigned char> out = src;
    cimg_forXY(out, x, y) {
        for (int c = 0; c < out.spectrum(); ++c) {
            int v = (int)round((out(x, y, c) - 128.0f) * factor + 128.0f);
            out(x, y, c) = (unsigned char)clampv(v, 0, 255);
        }
    }
    return out;
}

CImg<unsigned char> op_negative(const CImg<unsigned char>& src) {
    CImg<unsigned char> out = src;
    cimg_forXY(out, x, y) {
        for (int c = 0; c < out.spectrum(); ++c)
            out(x, y, c) = (unsigned char)(255 - out(x, y, c));
    }
    return out;
}

CImg<unsigned char> op_rgb_add(const CImg<unsigned char>& src, int rAdd, int gAdd, int bAdd) {
    CImg<unsigned char> out = src;
    int s = out.spectrum();
    cimg_forXY(out, x, y) {
        if (s >= 1) out(x, y, 0) = (unsigned char)clampv((int)out(x, y, 0) + rAdd, 0, 255);
        if (s >= 2) out(x, y, 1) = (unsigned char)clampv((int)out(x, y, 1) + gAdd, 0, 255);
        if (s >= 3) out(x, y, 2) = (unsigned char)clampv((int)out(x, y, 2) + bAdd, 0, 255);
    }
    return out;
}
