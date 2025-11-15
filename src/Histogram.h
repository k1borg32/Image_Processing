#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "Utils.h"
#include <vector>

// Compute histogram for a single channel
std::vector<int> compute_histogram(const CImg<unsigned char>& src, int channel);

// H4: Power 2/3 equalization
CImg<unsigned char> histogram_power23(const CImg<unsigned char>& src, 
                                       int gmin = 0, int gmax = 255);

// Save histogram as image
void save_histogram_image(const std::vector<int>& hist, 
                          const std::string& outputPath);

// Image characteristics structure
struct ImageCharacteristics {
    double mean;           // C1
    double variance;       // C1
    double stdev;          // C2
    double varcoeff_I;     // C2
    double asymmetry;      // C3
    double flattening;     // C4
    double varcoeff_II;    // C5
    double entropy;        // C6
};

// Compute all characteristics
ImageCharacteristics compute_characteristics(const CImg<unsigned char>& src, 
                                             int channel = 0);

#endif
