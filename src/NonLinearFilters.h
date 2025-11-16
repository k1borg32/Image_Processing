#ifndef NONLINEAR_FILTERS_H
#define NONLINEAR_FILTERS_H

#include "Utils.h"

CImg<unsigned char> roberts_operator_I (const CImg<unsigned char>& src);
CImg<unsigned char> roberts_operator_II(const CImg<unsigned char>& src);
CImg<unsigned char> sobel_operator      (const CImg<unsigned char>& src);
CImg<unsigned char> kirsch_operator     (const CImg<unsigned char>& src);
CImg<unsigned char> rosenfeld_operator  (const CImg<unsigned char>& src, int P = 1);
CImg<unsigned char> ll_operator         (const CImg<unsigned char>& src);

#endif
