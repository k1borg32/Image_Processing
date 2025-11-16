#ifndef LINEAR_FILTERS_H
#define LINEAR_FILTERS_H

#include "Utils.h"

CImg<unsigned char> convolve_universal(const CImg<unsigned char>& src,
                                       const std::vector<std::vector<float>>& kernel);
CImg<unsigned char> edge_sharpen_type1(const CImg<unsigned char>& src);
CImg<unsigned char> edge_sharpen_type2(const CImg<unsigned char>& src);
CImg<unsigned char> edge_sharpen_type3(const CImg<unsigned char>& src);
CImg<unsigned char> edge_sharpen_optimized(const CImg<unsigned char>& src);

#endif
