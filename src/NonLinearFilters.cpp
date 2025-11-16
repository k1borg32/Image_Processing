#include "NonLinearFilters.h"
#include <cstdlib>
#include <stdexcept>

using namespace std;

CImg<unsigned char> rosenfeld_operator(const CImg<unsigned char>& src, int P) {
    if (P <= 0) {
        throw runtime_error("--orosenfeld expects P > 0");
    }
    const int w = src.width();
    const int h = src.height();
    const int s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);

    for (int c = 0; c < s; ++c) {
        cimg_forXY(src, x, y) {
            int positive = 0;
            int negative = 0;
            for (int i = 0; i < P; ++i) {
                positive += getp(src, x + i, y, c);
            }
            for (int i = 1; i <= P; ++i) {
                negative += getp(src, x - i, y, c);
            }
            int diff = std::abs(positive - negative) / P;
            out(x, y, c) = static_cast<unsigned char>(clampv(diff, 0, 255));
        }
    }
    return out;
}
