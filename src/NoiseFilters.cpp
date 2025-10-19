#include "NoiseFilters.h"

using namespace std;
using namespace cimg_library;

static int median_of(vector<int>& v) {
    size_t n = v.size(), mid = n / 2;
    nth_element(v.begin(), v.begin() + mid, v.end());
    int med = v[mid];
    if (n % 2 == 0) {
        nth_element(v.begin(), v.begin() + mid - 1, v.end());
        med = (v[mid - 1] + v[mid]) / 2;
    }
    return med;
}

CImg<unsigned char> op_amean(const CImg<unsigned char>& src, int ksize) {
    if (!isOdd(ksize) || ksize < 3) throw runtime_error("-ksize must be odd and >= 3");
    int r = ksize / 2;
    int w = src.width(), h = src.height(), s = src.spectrum();
    const int windowArea = ksize * ksize;
    CImg<unsigned char> out(w, h, 1, s, 0);

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            for (int c = 0; c < s; ++c) {
                int sum = 0;
                for (int dy = -r; dy <= r; ++dy)
                    for (int dx = -r; dx <= r; ++dx)
                        sum += getp(src, x + dx, y + dy, c);
                int v = (int)round((double)sum / windowArea);
                out(x, y, c) = (unsigned char)clampv(v, 0, 255);
            }
    return out;
}

CImg<unsigned char> op_adaptive_median(const CImg<unsigned char>& src, int ksize_start, int smax) {
    if (!isOdd(ksize_start) || ksize_start < 3) throw runtime_error("-ksize must be odd and >= 3");
    if (!isOdd(smax) || smax < ksize_start) throw runtime_error("-smax must be odd and >= -ksize");
    int w = src.width(), h = src.height(), s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s, 0);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < s; ++c) {
                int zxy = src(x, y, c);
                int currentK = ksize_start;
                int outputValue = zxy;
                bool decided = false;

                while (!decided) {
                    int r = currentK / 2;
                    vector<int> window;
                    window.reserve(currentK * currentK);
                    int zmin = 255, zmax = 0;
                    for (int dy = -r; dy <= r; ++dy)
                        for (int dx = -r; dx <= r; ++dx) {
                            int val = getp(src, x + dx, y + dy, c);
                            window.push_back(val);
                            if (val < zmin) zmin = val;
                            if (val > zmax) zmax = val;
                        }
                    int zmed = median_of(window);
                    int A1 = zmed - zmin;
                    int A2 = zmed - zmax;

                    if (A1 > 0 && A2 < 0) {
                        int B1 = zxy - zmin;
                        int B2 = zxy - zmax;
                        outputValue = (B1 > 0 && B2 < 0) ? zxy : zmed;
                        decided = true;
                    } else {
                        currentK += 2;
                        if (currentK > smax) {
                            outputValue = zxy;
                            decided = true;
                        }
                    }
                }
                out(x, y, c) = (unsigned char)outputValue;
            }
        }
    }
    return out;
}
