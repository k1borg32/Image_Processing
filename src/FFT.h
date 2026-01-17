#ifndef FFT_H
#define FFT_H

#include <vector>
#include <complex>

// using complex doubles for FFT computations
typedef std::complex<double> Complex;
typedef std::vector<Complex> ComplexVector;
typedef std::vector<std::vector<Complex>> ComplexMatrix;

// =============================================================================
// T2: Fast Fourier Transform with Decimation in Frequency Domain (DIF)
// =============================================================================

// 1D FFT - Sande-Tukey DIF algorithm
ComplexVector fft_1d_dif(const ComplexVector& input);

// 1D Inverse FFT using DIF
ComplexVector ifft_1d_dif(const ComplexVector& input);

// 2D FFT using separability (rows then columns)
ComplexMatrix fft_2d(const ComplexMatrix& input);

// 2D Inverse FFT
ComplexMatrix ifft_2d(const ComplexMatrix& input);

// =============================================================================
// "Slow" DFT directly from definition (for verification)
// =============================================================================

ComplexVector dft_slow(const ComplexVector& input);
ComplexVector idft_slow(const ComplexVector& input);

// =============================================================================
// Utility functions
// =============================================================================

// shift zero frequency to center (swap quadrants)
ComplexMatrix fft_shift(const ComplexMatrix& input);

// pad to next power of 2
size_t next_power_of_2(size_t n);

// helper to convert image to complex matrix
ComplexMatrix image_to_complex(const unsigned char* data, int width, int height);

// helper to extract real part back to image
void complex_to_image(const ComplexMatrix& data, unsigned char* output, int width, int height);

// get magnitude spectrum (for visualization)
void get_magnitude_spectrum(const ComplexMatrix& spectrum, double* output, int width, int height, bool log_scale = true);

#endif
