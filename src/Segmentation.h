#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "Utils.h"

CImg<unsigned char> region_growing(const CImg<unsigned char>& src, int seedX, int seedY, int threshold);

#endif
