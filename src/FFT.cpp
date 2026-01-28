#include "FFT.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

size_t next_power_of_2(size_t n) {
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

size_t bit_reverse(size_t x, int log2n) {
    size_t result = 0;
    for (int i = 0; i < log2n; i++) {
        result <<= 1;
        result |= (x & 1);
        x >>= 1;
    }
    return result;
}

// ===== 1D FFT using Decimation in Frequency (DIF) =====
ComplexVector fft_1d_dif_recursive(ComplexVector& x) {
    size_t N = x.size();
    if (N == 1) return x;
    
    size_t half = N / 2;
    ComplexVector sums(half);
    ComplexVector diffs(half);
    
    for (size_t n = 0; n < half; n++) {
        Complex top = x[n];
        Complex bot = x[n + half];
        Complex W = std::polar(1.0, -2.0 * M_PI * n / N);  // twiddle factor
        
        sums[n] = top + bot;
        diffs[n] = (top - bot) * W;
    }
    
    ComplexVector even = fft_1d_dif_recursive(sums);
    ComplexVector odd = fft_1d_dif_recursive(diffs);
    
    ComplexVector X(N);
    for (size_t k = 0; k < half; k++) {
        X[2*k] = even[k];
        X[2*k + 1] = odd[k];
    }
    return X;
}

ComplexVector fft_1d_dif(const ComplexVector& input) {
    size_t n = input.size();
    size_t N = next_power_of_2(n);
    
    ComplexVector x(N, Complex(0, 0));
    for (size_t i = 0; i < n; i++) x[i] = input[i];
    
    return fft_1d_dif_recursive(x);
}

// ===== 1D Inverse FFT (positive exponent, scale by 1/N) =====
ComplexVector ifft_1d_dif_recursive(ComplexVector& x) {
    size_t N = x.size();
    if (N == 1) return x;
    
    size_t half = N / 2;
    ComplexVector sums(half);
    ComplexVector diffs(half);
    
    for (size_t n = 0; n < half; n++) {
        Complex top = x[n];
        Complex bot = x[n + half];
        Complex W = std::polar(1.0, 2.0 * M_PI * n / N);  // positive exponent for inverse
        
        sums[n] = top + bot;
        diffs[n] = (top - bot) * W;
    }
    
    ComplexVector even = ifft_1d_dif_recursive(sums);
    ComplexVector odd = ifft_1d_dif_recursive(diffs);
    
    ComplexVector X(N);
    for (size_t k = 0; k < half; k++) {
        X[2*k] = even[k];
        X[2*k + 1] = odd[k];
    }
    return X;
}

ComplexVector ifft_1d_dif(const ComplexVector& input) {
    size_t n = input.size();
    size_t N = next_power_of_2(n);
    
    ComplexVector x(N, Complex(0, 0));
    for (size_t i = 0; i < n; i++) x[i] = input[i];
    
    ComplexVector result = ifft_1d_dif_recursive(x);
    for (size_t i = 0; i < N; i++) result[i] /= N;  // scale by 1/N
    
    return result;
}

// ===== Slow DFT from definition (O(N^2) - for verification) =====
ComplexVector dft_slow(const ComplexVector& input) {
    size_t N = input.size();
    ComplexVector X(N);
    
    for (size_t k = 0; k < N; k++) {
        X[k] = Complex(0, 0);
        for (size_t n = 0; n < N; n++) {
            double angle = -2.0 * M_PI * n * k / N;
            X[k] += input[n] * std::polar(1.0, angle);
        }
    }
    return X;
}

ComplexVector idft_slow(const ComplexVector& input) {
    size_t N = input.size();
    ComplexVector x(N);
    
    for (size_t n = 0; n < N; n++) {
        x[n] = Complex(0, 0);
        for (size_t k = 0; k < N; k++) {
            double angle = 2.0 * M_PI * n * k / N;
            x[n] += input[k] * std::polar(1.0, angle);
        }
        x[n] /= N;
    }
    return x;
}

// ===== 2D FFT using separability: rows first, then columns =====
ComplexMatrix fft_2d(const ComplexMatrix& input) {
    if (input.empty() || input[0].empty()) return input;
    
    size_t rows = input.size();
    size_t cols = input[0].size();
    size_t N = next_power_of_2(rows);
    size_t M = next_power_of_2(cols);
    
    // Pad to power of 2
    ComplexMatrix padded(N, ComplexVector(M, Complex(0, 0)));
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            padded[i][j] = input[i][j];
    
    // Step 1: FFT each row
    ComplexMatrix temp(N, ComplexVector(M));
    for (size_t i = 0; i < N; i++)
        temp[i] = fft_1d_dif(padded[i]);
    
    // Step 2: FFT each column
    ComplexMatrix result(N, ComplexVector(M));
    for (size_t j = 0; j < M; j++) {
        ComplexVector col(N);
        for (size_t i = 0; i < N; i++) col[i] = temp[i][j];
        
        ComplexVector col_fft = fft_1d_dif(col);
        
        for (size_t i = 0; i < N; i++) result[i][j] = col_fft[i];
    }
    return result;
}

// ===== 2D Inverse FFT =====
ComplexMatrix ifft_2d(const ComplexMatrix& input) {
    if (input.empty() || input[0].empty()) return input;
    
    size_t N = input.size();
    size_t M = input[0].size();
    
    // Step 1: IFFT each row
    ComplexMatrix temp(N, ComplexVector(M));
    for (size_t i = 0; i < N; i++)
        temp[i] = ifft_1d_dif(input[i]);
    
    // Step 2: IFFT each column
    ComplexMatrix result(N, ComplexVector(M));
    for (size_t j = 0; j < M; j++) {
        ComplexVector col(N);
        for (size_t i = 0; i < N; i++) col[i] = temp[i][j];
        
        ComplexVector col_ifft = ifft_1d_dif(col);
        
        for (size_t i = 0; i < N; i++) result[i][j] = col_ifft[i];
    }
    return result;
}

// ===== FFT Shift: move DC component to center (swap quadrants) =====
ComplexMatrix fft_shift(const ComplexMatrix& input) {
    if (input.empty()) return input;
    
    size_t N = input.size();
    size_t M = input[0].size();
    size_t halfN = N / 2;
    size_t halfM = M / 2;
    
    ComplexMatrix result(N, ComplexVector(M));
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < M; j++) {
            size_t ni = (i + halfN) % N;
            size_t nj = (j + halfM) % M;
            result[ni][nj] = input[i][j];
        }
    }
    return result;
}

// ===== Image conversion utilities =====
ComplexMatrix image_to_complex(const unsigned char* data, int width, int height) {
    ComplexMatrix result(height, ComplexVector(width));
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            result[y][x] = Complex(data[y * width + x], 0);
    return result;
}

void complex_to_image(const ComplexMatrix& data, unsigned char* output, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double val = data[y][x].real();
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            output[y * width + x] = (unsigned char)(val + 0.5);
        }
    }
}

// ===== Magnitude spectrum for visualization =====
void get_magnitude_spectrum(const ComplexMatrix& spectrum, double* output, int width, int height, bool log_scale) {
    double maxVal = 0;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double mag = std::abs(spectrum[y][x]);
            if (log_scale) mag = std::log(1 + mag);
            output[y * width + x] = mag;
            if (mag > maxVal) maxVal = mag;
        }
    }
    
    if (maxVal > 0) {
        double scale = 255.0 / maxVal;
        for (int i = 0; i < width * height; i++)
            output[i] *= scale;
    }
}
