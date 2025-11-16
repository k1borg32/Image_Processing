#include "LinearFilters.h"
#include <cmath>

using namespace std;

namespace {

float apply_kernel_at(const CImg<unsigned char>& src,
                      int x,
                      int y,
                      int channel,
                      const vector<vector<float>>& kernel) {
    const int rows = static_cast<int>(kernel.size());
    const int cols = rows > 0 ? static_cast<int>(kernel[0].size()) : 0;
    const int rowRadius = rows / 2;
    const int colRadius = cols / 2;

    float sum = 0.0f;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int offsetY = i - rowRadius;
            int offsetX = j - colRadius;
            sum += kernel[i][j] * getp(src, x + offsetX, y + offsetY, channel);
        }
    }
    return sum;
}

}

CImg<unsigned char> convolve_universal(const CImg<unsigned char>& src,
                                       const vector<vector<float>>& kernel) {
    const int w = src.width();
    const int h = src.height();
    const int s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);

    for (int channel = 0; channel < s; ++channel) {
        cimg_forXY(src, x, y) {
            float response = apply_kernel_at(src, x, y, channel, kernel);
            out(x, y, channel) = static_cast<unsigned char>(clampv(static_cast<int>(std::round(response)), 0, 255));
        }
    }
    return out;
}

CImg<unsigned char> edge_sharpen_type1(const CImg<unsigned char>& src) {
    static const vector<vector<float>> kernel = {
        {0.0f, -1.0f, 0.0f},
        {-1.0f, 5.0f, -1.0f},
        {0.0f, -1.0f, 0.0f}
    };
    return convolve_universal(src, kernel);
}

CImg<unsigned char> edge_sharpen_type2(const CImg<unsigned char>& src) {
    static const vector<vector<float>> kernel = {
        {-1.0f, -1.0f, -1.0f},
        {-1.0f,  9.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f}
    };
    return convolve_universal(src, kernel);
}

CImg<unsigned char> edge_sharpen_type3(const CImg<unsigned char>& src) {
    static const vector<vector<float>> kernel = {
        {1.0f, -2.0f, 1.0f},
        {-2.0f, 5.0f, -2.0f},
        {1.0f, -2.0f, 1.0f}
    };
    return convolve_universal(src, kernel);
}

CImg<unsigned char> edge_sharpen_optimized(const CImg<unsigned char>& src) {
    const int w = src.width();
    const int h = src.height();
    const int s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);

    for (int channel = 0; channel < s; ++channel) {
        cimg_forXY(src, x, y) {
            int original = src(x, y, channel);
            int sum = getp(src, x - 1, y, channel) + getp(src, x + 1, y, channel) +
                      getp(src, x, y - 1, channel) + getp(src, x, y + 1, channel);
            int avg = sum / 4;
            int sharpened = original + (original - avg);
            out(x, y, channel) = static_cast<unsigned char>(clampv(sharpened, 0, 255));
        }
    }
    return out;
}
