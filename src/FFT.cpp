#include "FFT.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// find next power of 2 >= n
size_t next_power_of_2(size_t n) {
    size_t p = 1;
    while (p < n) p <<= 1;
    return p;
}

// bit reversal - needed for iterative FFT
size_t bit_reverse(size_t x, int log2n) {
    size_t result = 0;
    for (int i = 0; i < log2n; i++) {
        result <<= 1;
        result |= (x & 1);
        x >>= 1;
    }
    return result;
}

// =============================================================================
// T2: Decimation in Frequency Domain (DIF) - RECURSIVE VERSION
// =============================================================================
// from the task formulas:
//   X(2k)   = DFT_{N/2}{ x(n) + x(n + N/2) }               for k = 0..N/2-1
//   X(2k+1) = DFT_{N/2}{ (x(n) - x(n + N/2)) * W_N^n }     for k = 0..N/2-1
//
// so we:
// 1. compute sums:        s(n) = x(n) + x(n + N/2)    for n = 0..N/2-1
// 2. compute differences: d(n) = (x(n) - x(n + N/2)) * W_N^n
// 3. recursively DFT both
// 4. interleave: even indices get DFT of sums, odd indices get DFT of diffs

ComplexVector fft_1d_dif_recursive(ComplexVector& x) {
    size_t N = x.size();
    
    // base case
    if (N == 1) return x;
    
    size_t half = N / 2;
    
    // prepare two halves for recursive calls
    ComplexVector sums(half);    // will become even-indexed outputs
    ComplexVector diffs(half);   // will become odd-indexed outputs
    
    for (size_t n = 0; n < half; n++) {
        Complex top = x[n];           // x(n)
        Complex bot = x[n + half];    // x(n + N/2)
        
        // twiddle factor W_N^n = e^(-j*2*pi*n/N)
        Complex W = std::polar(1.0, -2.0 * M_PI * n / N);
        
        sums[n] = top + bot;              // x(n) + x(n + N/2)
        diffs[n] = (top - bot) * W;       // (x(n) - x(n + N/2)) * W_N^n
    }
    
    // recursively compute DFT of both halves
    ComplexVector even = fft_1d_dif_recursive(sums);   // becomes X(2k)
    ComplexVector odd = fft_1d_dif_recursive(diffs);   // becomes X(2k+1)
    
    // interleave results
    ComplexVector X(N);
    for (size_t k = 0; k < half; k++) {
        X[2*k] = even[k];       // even indices
        X[2*k + 1] = odd[k];    // odd indices
    }
    
    return X;
}

// wrapper that handles padding to power of 2
ComplexVector fft_1d_dif(const ComplexVector& input) {
    size_t n = input.size();
    size_t N = next_power_of_2(n);
    
    // copy and pad with zeros
    ComplexVector x(N, Complex(0, 0));
    for (size_t i = 0; i < n; i++) x[i] = input[i];
    
    return fft_1d_dif_recursive(x);
}

// =============================================================================
// Inverse FFT using DIF (recursive)
// =============================================================================
// same as forward but with positive exponent, then scale by 1/N

ComplexVector ifft_1d_dif_recursive(ComplexVector& x) {
    size_t N = x.size();
    
    if (N == 1) return x;
    
    size_t half = N / 2;
    
    ComplexVector sums(half);
    ComplexVector diffs(half);
    
    for (size_t n = 0; n < half; n++) {
        Complex top = x[n];
        Complex bot = x[n + half];
        
        // POSITIVE exponent for inverse: W_N^{-n} = e^(+j*2*pi*n/N)
        Complex W = std::polar(1.0, 2.0 * M_PI * n / N);
        
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
    
    // scale by 1/N for inverse
    for (size_t i = 0; i < N; i++) {
        result[i] /= N;
    }
    
    return result;
}

// =============================================================================
// "Slow" DFT directly from definition (for verification/comparison)
// =============================================================================
// X(k) = sum_{n=0}^{N-1} x(n) * W_N^{nk}
// where W_N^{nk} = e^{-j*2*pi*nk/N}

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

// slow IDFT from definition
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

// =============================================================================
// 2D FFT using separability (rows then columns)
// =============================================================================

ComplexMatrix fft_2d(const ComplexMatrix& input) {
    if (input.empty() || input[0].empty()) return input;
    
    size_t rows = input.size();
    size_t cols = input[0].size();
    
    // pad to power of 2
    size_t N = next_power_of_2(rows);
    size_t M = next_power_of_2(cols);
    
    // padded matrix
    ComplexMatrix padded(N, ComplexVector(M, Complex(0, 0)));
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            padded[i][j] = input[i][j];
        }
    }
    
    // step 1: M one-dimensional N-point transforms of rows
    ComplexMatrix temp(N, ComplexVector(M));
    for (size_t i = 0; i < N; i++) {
        temp[i] = fft_1d_dif(padded[i]);
    }
    
    // step 2: N one-dimensional M-point transforms of columns
    ComplexMatrix result(N, ComplexVector(M));
    for (size_t j = 0; j < M; j++) {
        // extract column
        ComplexVector col(N);
        for (size_t i = 0; i < N; i++) col[i] = temp[i][j];
        
        // transform
        ComplexVector col_fft = fft_1d_dif(col);
        
        // put back
        for (size_t i = 0; i < N; i++) result[i][j] = col_fft[i];
    }
    
    return result;
}

ComplexMatrix ifft_2d(const ComplexMatrix& input) {
    if (input.empty() || input[0].empty()) return input;
    
    size_t N = input.size();
    size_t M = input[0].size();
    
    // step 1: IFFT each row
    ComplexMatrix temp(N, ComplexVector(M));
    for (size_t i = 0; i < N; i++) {
        temp[i] = ifft_1d_dif(input[i]);
    }
    
    // step 2: IFFT each column
    ComplexMatrix result(N, ComplexVector(M));
    for (size_t j = 0; j < M; j++) {
        ComplexVector col(N);
        for (size_t i = 0; i < N; i++) col[i] = temp[i][j];
        
        ComplexVector col_ifft = ifft_1d_dif(col);
        
        for (size_t i = 0; i < N; i++) result[i][j] = col_ifft[i];
    }
    
    return result;
}

// shift spectrum so DC is at center - swap quadrants diagonally
ComplexMatrix fft_shift(const ComplexMatrix& input) {
    if (input.empty()) return input;
    
    size_t N = input.size();
    size_t M = input[0].size();
    size_t halfN = N / 2;
    size_t halfM = M / 2;
    
    ComplexMatrix result(N, ComplexVector(M));
    
    // swap quadrants:
    // Q1 <-> Q3 (top-left with bottom-right)
    // Q2 <-> Q4 (top-right with bottom-left)
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < M; j++) {
            size_t ni = (i + halfN) % N;
            size_t nj = (j + halfM) % M;
            result[ni][nj] = input[i][j];
        }
    }
    
    return result;
}

// convert grayscale image data to complex matrix
ComplexMatrix image_to_complex(const unsigned char* data, int width, int height) {
    ComplexMatrix result(height, ComplexVector(width));
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = Complex(data[y * width + x], 0);
        }
    }
    return result;
}

// extract real part and clamp to 0-255
void complex_to_image(const ComplexMatrix& data, unsigned char* output, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double val = data[y][x].real();
            // clamp to valid range
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            output[y * width + x] = (unsigned char)(val + 0.5);
        }
    }
}

// get magnitude spectrum for visualization (usually log scale looks better)
void get_magnitude_spectrum(const ComplexMatrix& spectrum, double* output, int width, int height, bool log_scale) {
    double maxVal = 0;
    
    // first pass: get magnitudes and find max
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double mag = std::abs(spectrum[y][x]);
            if (log_scale) mag = std::log(1 + mag);  // log scale to see detail
            output[y * width + x] = mag;
            if (mag > maxVal) maxVal = mag;
        }
    }
    
    // normalize to 0-255 range
    if (maxVal > 0) {
        double scale = 255.0 / maxVal;
        for (int i = 0; i < width * height; i++) {
            output[i] *= scale;
        }
    }
}
