#include "FrequencyFilters.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double distance_from_center(int x, int y, int centerX, int centerY) {
    double dx = x - centerX;
    double dy = y - centerY;
    return std::sqrt(dx*dx + dy*dy);
}

// ===== F1: Low-pass filter (pass frequencies inside radius R) =====
ComplexMatrix filter_lowpass(int width, int height, double R) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double d = distance_from_center(x, y, centerX, centerY);
            mask[y][x] = (d <= R) ? Complex(1, 0) : Complex(0, 0);
        }
    }
    return mask;
}

// ===== F2: High-pass filter (pass frequencies outside radius R, protect DC) =====
ComplexMatrix filter_highpass(int width, int height, double R) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == centerX && y == centerY) {
                mask[y][x] = Complex(1, 0);  // protect DC
                continue;
            }
            double d = distance_from_center(x, y, centerX, centerY);
            mask[y][x] = (d > R) ? Complex(1, 0) : Complex(0, 0);
        }
    }
    return mask;
}

// ===== F3: Band-pass filter (pass frequencies between Rmin and Rmax) =====
ComplexMatrix filter_bandpass(int width, int height, double Rmin, double Rmax) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == centerX && y == centerY) {
                mask[y][x] = Complex(1, 0);  // protect DC
                continue;
            }
            double d = distance_from_center(x, y, centerX, centerY);
            mask[y][x] = (d >= Rmin && d <= Rmax) ? Complex(1, 0) : Complex(0, 0);
        }
    }
    return mask;
}

// ===== F4: Band-cut filter (block frequencies between Rmin and Rmax) =====
ComplexMatrix filter_bandcut(int width, int height, double Rmin, double Rmax) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double d = distance_from_center(x, y, centerX, centerY);
            mask[y][x] = (d >= Rmin && d <= Rmax) ? Complex(0, 0) : Complex(1, 0);
        }
    }
    return mask;
}

// ===== F5: High-pass with direction (wedge filter) =====
ComplexMatrix filter_highpass_direction(int width, int height, double R, double theta, double phi) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    double thetaRad = theta * M_PI / 180.0;
    double halfPhi = (phi * M_PI / 180.0) / 2.0;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == centerX && y == centerY) {
                mask[y][x] = Complex(1, 0);  // protect DC
                continue;
            }
            
            double d = distance_from_center(x, y, centerX, centerY);
            if (d <= R) {
                mask[y][x] = Complex(0, 0);  // high-pass: block inside R
                continue;
            }
            
            double dx = x - centerX;
            double dy = y - centerY;
            double angle = std::atan2(dy, dx);
            
            // Check if angle is within wedge
            double diff = angle - thetaRad;
            while (diff > M_PI) diff -= 2 * M_PI;
            while (diff < -M_PI) diff += 2 * M_PI;
            bool inWedge = (std::abs(diff) <= halfPhi);
            
            // Also check opposite side (FFT symmetry)
            double diffOpp = angle - (thetaRad + M_PI);
            while (diffOpp > M_PI) diffOpp -= 2 * M_PI;
            while (diffOpp < -M_PI) diffOpp += 2 * M_PI;
            bool inWedgeOpp = (std::abs(diffOpp) <= halfPhi);
            
            mask[y][x] = (inWedge || inWedgeOpp) ? Complex(1, 0) : Complex(0, 0);
        }
    }
    return mask;
}

// ===== F6: Phase modifying filter (shifts image by k,l pixels) =====
ComplexMatrix filter_phase_modify(int width, int height, int k, int l) {
    ComplexMatrix mask(height, ComplexVector(width));
    
    for (int n = 0; n < height; n++) {
        for (int m = 0; m < width; m++) {
            double phase = -n * k * 2.0 * M_PI / height 
                          - m * l * 2.0 * M_PI / width 
                          + (k + l) * M_PI;
            mask[n][m] = std::polar(1.0, phase);
        }
    }
    return mask;
}

// ===== Apply filter mask to spectrum (element-wise multiplication) =====
ComplexMatrix apply_filter(const ComplexMatrix& spectrum, const ComplexMatrix& mask) {
    size_t N = spectrum.size();
    size_t M = spectrum[0].size();
    
    ComplexMatrix result(N, ComplexVector(M));
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < M; j++)
            result[i][j] = spectrum[i][j] * mask[i][j];
    return result;
}

ComplexMatrix apply_phase_filter(const ComplexMatrix& spectrum, const ComplexMatrix& phaseMask) {
    return apply_filter(spectrum, phaseMask);
}
