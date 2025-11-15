#ifndef LINEAR_FILTERS_H
#define LINEAR_FILTERS_H

#include "Utils.h"

// Universal convolution (works with any mask)
CImg<unsigned char> convolve_universal(const CImg<unsigned char>& src,
                                       const std::vector<std::vector<float>>& kernel);

// S2: Edge sharpening variants
CImg<unsigned char> edge_sharpen_type1(const CImg<unsigned char>& src);
CImg<unsigned char> edge_sharpen_type2(const CImg<unsigned char>& src);
CImg<unsigned char> edge_sharpen_type3(const CImg<unsigned char>& src);

// Optimized edge sharpening
CImg<unsigned char> edge_sharpen_optimized(const CImg<unsigned char>& src);

#endif
