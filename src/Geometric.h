#ifndef GEOMETRIC_H
#define GEOMETRIC_H
#include "Utils.h"

using namespace cimg_library;

CImg<unsigned char> op_hflip(const CImg<unsigned char>& src);
CImg<unsigned char> op_vflip(const CImg<unsigned char>& src);
CImg<unsigned char> op_dflip(const CImg<unsigned char>& src);
CImg<unsigned char> op_shrink(const CImg<unsigned char>& src, float factor);
CImg<unsigned char> op_enlarge(const CImg<unsigned char>& src, float factor);

#endif
