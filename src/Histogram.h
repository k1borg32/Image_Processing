#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "Utils.h"
#include <vector>

std::vector<int> compute_histogram(const CImg<unsigned char>& src, int channel);

CImg<unsigned char> histogram_uniform   (const CImg<unsigned char>& src,
                                         int gmin = 0, int gmax = 255);
CImg<unsigned char> histogram_exponential(const CImg<unsigned char>& src,
                                         float alpha = 1.0f,
                                         int gmin = 0, int gmax = 255);
CImg<unsigned char> histogram_rayleigh   (const CImg<unsigned char>& src,
                                         float alpha = 45.0f,
                                         int gmin = 0, int gmax = 255);
CImg<unsigned char> histogram_power23    (const CImg<unsigned char>& src,
                                         int gmin = 0, int gmax = 255);
CImg<unsigned char> histogram_hyperbolic (const CImg<unsigned char>& src,
                                         int gmin = 0, int gmax = 255);

void save_histogram_image(const std::vector<int>& hist, 
                          const std::string& outputPath);

struct ImageCharacteristics {
    double mean;
    double variance;
    double stdev;
    double varcoeff_I;
    double asymmetry;
    double flattening;
    double varcoeff_II;
    double entropy;
};

ImageCharacteristics compute_characteristics(const CImg<unsigned char>& src, 
                                             int channel = 0);

#endif
