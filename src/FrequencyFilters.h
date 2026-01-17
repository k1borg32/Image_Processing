#ifndef FREQUENCY_FILTERS_H
#define FREQUENCY_FILTERS_H

#include "FFT.h"

// F1: Low-pass filter - keeps frequencies inside radius R
ComplexMatrix filter_lowpass(int width, int height, double R);

// F2: High-pass filter - keeps frequencies outside radius R
ComplexMatrix filter_highpass(int width, int height, double R);

// F3: Band-pass filter - keeps frequencies between Rmin and Rmax
ComplexMatrix filter_bandpass(int width, int height, double Rmin, double Rmax);

// F4: Band-cut filter - removes frequencies between Rmin and Rmax
ComplexMatrix filter_bandcut(int width, int height, double Rmin, double Rmax);

// F5: High-pass with edge direction (wedge/fan filter)
// theta = direction angle in degrees, phi = wedge width in degrees
ComplexMatrix filter_highpass_direction(int width, int height, double R, double theta, double phi);

// F6: Phase modifying filter - shifts the image by (k,l) pixels
ComplexMatrix filter_phase_modify(int width, int height, int k, int l);

// apply a filter mask to spectrum (element-wise multiply)
ComplexMatrix apply_filter(const ComplexMatrix& spectrum, const ComplexMatrix& mask);

// for F6 specifically - it modifies phase, not magnitude
ComplexMatrix apply_phase_filter(const ComplexMatrix& spectrum, const ComplexMatrix& phaseMask);

#endif
