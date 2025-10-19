#ifndef OPERATIONS_H
#define OPERATIONS_H
#include "Utils.h"

using namespace cimg_library;

CImg<unsigned char> op_brightness(const CImg<unsigned char>& src, int value);
CImg<unsigned char> op_contrast_linear(const CImg<unsigned char>& src, float factor);
CImg<unsigned char> op_negative(const CImg<unsigned char>& src);
CImg<unsigned char> op_rgb_add(const CImg<unsigned char>& src, int rAdd, int gAdd, int bAdd);

#endif
