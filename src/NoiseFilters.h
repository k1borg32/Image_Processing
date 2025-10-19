#ifndef NOISE_FILTERS_H
#define NOISE_FILTERS_H
#include "Utils.h"

using namespace cimg_library;

CImg<unsigned char> op_amean(const CImg<unsigned char>& src, int ksize);
CImg<unsigned char> op_adaptive_median(const CImg<unsigned char>& src, int ksize_start, int smax);

#endif
