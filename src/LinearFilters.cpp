#include "LinearFilters.h"
#include <iostream>

using namespace std;

CImg<unsigned char> convolve_universal(const CImg<unsigned char>& src,
                                       const vector<vector<float>>& kernel) {
    int w = src.width(), h = src.height(), s = src.spectrum();
    int M = kernel.size() / 2;  // Assuming square kernel
    CImg<unsigned char> out(w, h, 1, s);
    
    for (int c = 0; c < s; ++c) {
        cimg_forXY(src, x, y) {
            // Skip borders - just copy original
            if (x < M || y < M || x >= w - M || y >= h - M) {
                out(x, y, c) = src(x, y, c);
                continue;
            }
            
            // Apply convolution
            float sum = 0.0f;
            for (int i = -M; i <= M; ++i) {
                for (int j = -M; j <= M; ++j) {
                    sum += kernel[i + M][j + M] * src(x + j, y + i, c);
                }
            }
            
            out(x, y, c) = (unsigned char)clampv((int)round(sum), 0, 255);
        }
    }
    return out;
}

CImg<unsigned char> edge_sharpen_type1(const CImg<unsigned char>& src) {
    // Kernel: center=5, cross=-1
    vector<vector<float>> kernel = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };
    return convolve_universal(src, kernel);
}

CImg<unsigned char> edge_sharpen_type2(const CImg<unsigned char>& src) {
    // Kernel: center=9, all neighbors=-1
    vector<vector<float>> kernel = {
        {-1, -1, -1},
        {-1,  9, -1},
        {-1, -1, -1}
    };
    return convolve_universal(src, kernel);
}

CImg<unsigned char> edge_sharpen_type3(const CImg<unsigned char>& src) {
    // Kernel: center=5, diagonal=1, cross=-2
    vector<vector<float>> kernel = {
        {1, -2, 1},
        {-2, 5, -2},
        {1, -2, 1}
    };
    return convolve_universal(src, kernel);
}

CImg<unsigned char> edge_sharpen_optimized(const CImg<unsigned char>& src) {
    // Optimized version of type1: uses fewer multiplications
    // g = f + (f - lowpass(f))
    int w = src.width(), h = src.height(), s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);
    
    for (int c = 0; c < s; ++c) {
        cimg_forXY(src, x, y) {
            int original = src(x, y, c);
            
            // Compute average of 4 neighbors (cross pattern)
            int sum = getp(src, x-1, y, c) + getp(src, x+1, y, c) +
                      getp(src, x, y-1, c) + getp(src, x, y+1, c);
            int avg = sum / 4;
            
            // Sharpen: original + (original - average)
            int sharpened = original + (original - avg);
            out(x, y, c) = (unsigned char)clampv(sharpened, 0, 255);
        }
    }
    return out;
}
