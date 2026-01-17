#include "FrequencyFilters.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// helper: euclidean distance from center
static double distance_from_center(int x, int y, int centerX, int centerY) {
    double dx = x - centerX;
    double dy = y - centerY;
    return std::sqrt(dx*dx + dy*dy);
}

// F1: Low-pass - let through frequencies close to DC (center)
ComplexMatrix filter_lowpass(int width, int height, double R) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double d = distance_from_center(x, y, centerX, centerY);
            // inside radius = pass (1), outside = block (0)
            mask[y][x] = (d <= R) ? Complex(1, 0) : Complex(0, 0);
        }
    }
    return mask;
}

// F2: High-pass - opposite of lowpass, block DC and low frequencies
ComplexMatrix filter_highpass(int width, int height, double R) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double d = distance_from_center(x, y, centerX, centerY);
            // outside radius = pass, inside = block
            mask[y][x] = (d > R) ? Complex(1, 0) : Complex(0, 0);
        }
    }
    return mask;
}

// F3: Band-pass - only let through a ring of frequencies
ComplexMatrix filter_bandpass(int width, int height, double Rmin, double Rmax) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double d = distance_from_center(x, y, centerX, centerY);
            // between Rmin and Rmax = pass
            mask[y][x] = (d >= Rmin && d <= Rmax) ? Complex(1, 0) : Complex(0, 0);
        }
    }
    return mask;
}

// F4: Band-cut (notch) - remove a ring of frequencies
ComplexMatrix filter_bandcut(int width, int height, double Rmin, double Rmax) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double d = distance_from_center(x, y, centerX, centerY);
            // between Rmin and Rmax = block, otherwise pass
            mask[y][x] = (d >= Rmin && d <= Rmax) ? Complex(0, 0) : Complex(1, 0);
        }
    }
    return mask;
}

// F5: High-pass with direction - wedge/fan shaped filter
// useful for removing directional noise or emphasizing edges in certain direction
ComplexMatrix filter_highpass_direction(int width, int height, double R, double theta, double phi) {
    ComplexMatrix mask(height, ComplexVector(width));
    int centerX = width / 2;
    int centerY = height / 2;
    
    // convert angles to radians
    double thetaRad = theta * M_PI / 180.0;
    double phiRad = phi * M_PI / 180.0;
    double halfPhi = phiRad / 2.0;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double d = distance_from_center(x, y, centerX, centerY);
            
            // must be outside radius R (high-pass part)
            if (d <= R) {
                mask[y][x] = Complex(0, 0);
                continue;
            }
            
            // calculate angle from center
            double dx = x - centerX;
            double dy = y - centerY;
            double angle = std::atan2(dy, dx);
            
            // check if angle falls within wedge [theta - phi/2, theta + phi/2]
            // need to handle wraparound
            double diff = angle - thetaRad;
            // normalize to [-pi, pi]
            while (diff > M_PI) diff -= 2 * M_PI;
            while (diff < -M_PI) diff += 2 * M_PI;
            
            bool inWedge = (std::abs(diff) <= halfPhi);
            
            // also check opposite side (FFT is symmetric for real images)
            double diffOpp = angle - (thetaRad + M_PI);
            while (diffOpp > M_PI) diffOpp -= 2 * M_PI;
            while (diffOpp < -M_PI) diffOpp += 2 * M_PI;
            bool inWedgeOpp = (std::abs(diffOpp) <= halfPhi);
            
            mask[y][x] = (inWedge || inWedgeOpp) ? Complex(1, 0) : Complex(0, 0);
        }
    }
    return mask;
}

// F6: Phase modifying filter - shifts image by (k, l) pixels
// this is based on the shift property of Fourier transform
// shifting in spatial domain = phase multiplication in frequency domain
ComplexMatrix filter_phase_modify(int width, int height, int k, int l) {
    ComplexMatrix mask(height, ComplexVector(width));
    
    for (int n = 0; n < height; n++) {
        for (int m = 0; m < width; m++) {
            // P(n,m) = exp(j * (-n*k*2pi/N - m*l*2pi/M + (k+l)*pi))
            double phase = -n * k * 2.0 * M_PI / height 
                          - m * l * 2.0 * M_PI / width 
                          + (k + l) * M_PI;
            mask[n][m] = std::polar(1.0, phase);
        }
    }
    return mask;
}

// multiply spectrum by mask element-wise (for magnitude filters F1-F5)
ComplexMatrix apply_filter(const ComplexMatrix& spectrum, const ComplexMatrix& mask) {
    size_t N = spectrum.size();
    size_t M = spectrum[0].size();
    
    ComplexMatrix result(N, ComplexVector(M));
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < M; j++) {
            result[i][j] = spectrum[i][j] * mask[i][j];
        }
    }
    return result;
}

// for phase filter (F6) - same operation really, just multiply
ComplexMatrix apply_phase_filter(const ComplexMatrix& spectrum, const ComplexMatrix& phaseMask) {
    return apply_filter(spectrum, phaseMask);
}
