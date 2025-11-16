#ifndef NONLINEAR_FILTERS_H
#define NONLINEAR_FILTERS_H

#include "Utils.h"

CImg<unsigned char> rosenfeld_operator  (const CImg<unsigned char>& src, int P = 1);

#endif
