#include "NonLinearFilters.h"
#include <cmath>
#include <stdexcept>

using namespace std;

namespace {

struct Neighbourhood {
    int center;
    int a0, a1, a2, a3, a4, a5, a6, a7;
};

Neighbourhood sample_neighbourhood(const CImg<unsigned char>& src, int x, int y, int c) {
    Neighbourhood n;
    n.center = getp(src, x,     y,     c);
    n.a0 = getp(src, x - 1, y - 1, c);
    n.a1 = getp(src, x - 1, y,     c);
    n.a2 = getp(src, x - 1, y + 1, c);
    n.a3 = getp(src, x,     y + 1, c);
    n.a4 = getp(src, x + 1, y + 1, c);
    n.a5 = getp(src, x + 1, y,     c);
    n.a6 = getp(src, x + 1, y - 1, c);
    n.a7 = getp(src, x,     y - 1, c);
    return n;
}

}

CImg<unsigned char> roberts_operator_I(const CImg<unsigned char>& src) {
    const int w = src.width();
    const int h = src.height();
    const int s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);

    for (int c = 0; c < s; ++c) {
        cimg_forXY(src, x, y) {
            int g1 = getp(src, x, y, c) - getp(src, x + 1, y + 1, c);
            int g2 = getp(src, x, y + 1, c) - getp(src, x + 1, y, c);
            float magnitude = std::sqrt(static_cast<float>(g1 * g1 + g2 * g2));
            out(x, y, c) = static_cast<unsigned char>(clampv(static_cast<int>(std::round(magnitude)), 0, 255));
        }
    }
    return out;
}

CImg<unsigned char> roberts_operator_II(const CImg<unsigned char>& src) {
    const int w = src.width();
    const int h = src.height();
    const int s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);

    for (int c = 0; c < s; ++c) {
        cimg_forXY(src, x, y) {
            int g1 = getp(src, x, y, c) - getp(src, x + 1, y + 1, c);
            int g2 = getp(src, x, y + 1, c) - getp(src, x + 1, y, c);
            int value = std::abs(g1) + std::abs(g2);
            out(x, y, c) = static_cast<unsigned char>(clampv(value, 0, 255));
        }
    }
    return out;
}

CImg<unsigned char> sobel_operator(const CImg<unsigned char>& src) {
    const int w = src.width();
    const int h = src.height();
    const int s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);

    for (int c = 0; c < s; ++c) {
        cimg_forXY(src, x, y) {
            auto n = sample_neighbourhood(src, x, y, c);
            int gx = (n.a2 + 2 * n.a3 + n.a4) - (n.a0 + 2 * n.a7 + n.a6);
            int gy = (n.a0 + 2 * n.a1 + n.a2) - (n.a6 + 2 * n.a5 + n.a4);
            float magnitude = std::sqrt(static_cast<float>(gx * gx + gy * gy));
            out(x, y, c) = static_cast<unsigned char>(clampv(static_cast<int>(std::round(magnitude)), 0, 255));
        }
    }
    return out;
}

CImg<unsigned char> kirsch_operator(const CImg<unsigned char>& src) {
    const int w = src.width();
    const int h = src.height();
    const int s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);

    for (int c = 0; c < s; ++c) {
        cimg_forXY(src, x, y) {
            auto n = sample_neighbourhood(src, x, y, c);
            int neighbours[8] = {n.a0, n.a1, n.a2, n.a3, n.a4, n.a5, n.a6, n.a7};
            float maxResponse = 0.0f;
            for (int i = 0; i < 8; ++i) {
                int ssum = neighbours[i % 8] + neighbours[(i + 1) % 8] + neighbours[(i + 2) % 8];
                int tsum = neighbours[(i + 3) % 8] + neighbours[(i + 4) % 8] +
                           neighbours[(i + 5) % 8] + neighbours[(i + 6) % 8] + neighbours[(i + 7) % 8];
                float response = std::fabs(5.0f * ssum - 3.0f * tsum);
                if (response > maxResponse) maxResponse = response;
            }
            out(x, y, c) = static_cast<unsigned char>(clampv(static_cast<int>(std::round(maxResponse)), 0, 255));
        }
    }
    return out;
}

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

CImg<unsigned char> ll_operator(const CImg<unsigned char>& src) {
    const int w = src.width();
    const int h = src.height();
    const int s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);

    for (int c = 0; c < s; ++c) {
        cimg_forXY(src, x, y) {
            auto n = sample_neighbourhood(src, x, y, c);
            float numerator = std::pow(static_cast<float>(n.center) + 1.0f, 4.0f);
            float denominator = (static_cast<float>(n.a1) + 1.0f) *
                                (static_cast<float>(n.a3) + 1.0f) *
                                (static_cast<float>(n.a5) + 1.0f) *
                                (static_cast<float>(n.a7) + 1.0f);
            float ratio = numerator / std::max(denominator, 1.0f);
            float response = 0.25f * std::fabs(std::log(ratio));
            float scaled = std::min(response * 40.0f, 255.0f);
            out(x, y, c) = static_cast<unsigned char>(clampv(static_cast<int>(std::round(scaled)), 0, 255));
        }
    }
    return out;
}
