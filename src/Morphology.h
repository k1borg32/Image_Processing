#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include "Utils.h"
#include <vector>

// Structure to hold the mask
// 1 = Foreground, 0 = Background, -1 = Don't Care
struct StructElement {
    int width, height;
    std::vector<int> data; 
    int cx, cy; // Center coordinates

    StructElement(int w, int h, const std::vector<int>& d, int cx, int cy)
        : width(w), height(h), data(d), cx(cx), cy(cy) {}

    // Factory methods for required elements
    static StructElement createSquare3x3();      // (iii)
    static StructElement createCross3x3();       // (iv)
    static std::vector<StructElement> createGolayLSequence(); // (xii) for M5
};

// Basic Operations
CImg<unsigned char> morph_dilation(const CImg<unsigned char>& src, const StructElement& se);
CImg<unsigned char> morph_erosion(const CImg<unsigned char>& src, const StructElement& se);
CImg<unsigned char> morph_opening(const CImg<unsigned char>& src, const StructElement& se);
CImg<unsigned char> morph_closing(const CImg<unsigned char>& src, const StructElement& se);

// HMT: Hit-or-Miss Transform
// Matches specific pattern of 1s (foreground) and 0s (background)
CImg<unsigned char> morph_hmt(const CImg<unsigned char>& src, const StructElement& se);

// M5: Iterative Thinning
// Calculates N(A, {B1...Bn}) until stability
CImg<unsigned char> morph_thinning(const CImg<unsigned char>& src);

#endif
