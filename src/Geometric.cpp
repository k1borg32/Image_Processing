#include "Geometric.h"

using namespace cimg_library;
using namespace std;

static CImg<unsigned char> scale_nn(const CImg<unsigned char>& src, float factor) {
    if (factor <= 0.f) throw runtime_error("Scale factor must be > 0");
    int w = src.width(), h = src.height(), s = src.spectrum();
    int nw = max(1, (int)round(w * factor));
    int nh = max(1, (int)round(h * factor));
    CImg<unsigned char> out(nw, nh, 1, s, 0);
    for (int y = 0; y < nh; ++y) {
        int sy = clampv((int)round(y / factor), 0, h - 1);
        for (int x = 0; x < nw; ++x) {
            int sx = clampv((int)round(x / factor), 0, w - 1);
            for (int c = 0; c < s; ++c) out(x, y, c) = src(sx, sy, c);
        }
    }
    return out;
}

CImg<unsigned char> op_hflip(const CImg<unsigned char>& src) {
    CImg<unsigned char> out = src;
    int w = out.width(), h = out.height(), s = out.spectrum();
    for (int x = 0; x < w / 2; ++x) {
        int xr = w - 1 - x;
        for (int y = 0; y < h; ++y)
            for (int c = 0; c < s; ++c)
                swap(out(x, y, c), out(xr, y, c));
    }
    return out;
}

CImg<unsigned char> op_vflip(const CImg<unsigned char>& src) {
    CImg<unsigned char> out = src;
    int w = out.width(), h = out.height(), s = out.spectrum();
    for (int y = 0; y < h / 2; ++y) {
        int yr = h - 1 - y;
        for (int x = 0; x < w; ++x)
            for (int c = 0; c < s; ++c)
                swap(out(x, y, c), out(x, yr, c));
    }
    return out;
}

CImg<unsigned char> op_dflip(const CImg<unsigned char>& src) {
    int w = src.width(), h = src.height(), s = src.spectrum();
    CImg<unsigned char> out(h, w, 1, s, 0);
    for (int x = 0; x < w; ++x)
        for (int y = 0; y < h; ++y)
            for (int c = 0; c < s; ++c)
                out(y, x, c) = src(x, y, c);
    return out;
}

CImg<unsigned char> op_shrink(const CImg<unsigned char>& src, float factor) {
    if (factor <= 0.0f || factor >= 1.0f) throw runtime_error("-factor must be in (0,1) for --shrink");
    return scale_nn(src, factor);
}

CImg<unsigned char> op_enlarge(const CImg<unsigned char>& src, float factor) {
    if (factor <= 1.0f) throw runtime_error("-factor must be > 1 for --enlarge");
    return scale_nn(src, factor);
}
