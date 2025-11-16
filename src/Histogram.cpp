#include "Histogram.h"
#include <cmath>
#include <algorithm>
#include <functional>
#include <stdexcept>

using namespace std;

namespace {

void validate_range(int gmin, int gmax) {
    if (gmin < 0 || gmax > 255 || gmin > gmax) {
        throw runtime_error("Histogram output range expects 0 <= gmin <= gmax <= 255");
    }
}

vector<float> build_cdf(const vector<int>& hist, int totalPixels) {
    vector<float> cdf(256, 0.0f);
    if (totalPixels <= 0) {
        return cdf;
    }
    float cumulative = 0.0f;
    for (int i = 0; i < 256; ++i) {
        cumulative += static_cast<float>(hist[i]) / static_cast<float>(totalPixels);
        if (cumulative > 1.0f) cumulative = 1.0f;
        cdf[i] = cumulative;
    }
    return cdf;
}

vector<unsigned char> build_lut(const vector<float>& cdf, const function<float(float)>& mapFn) {
    vector<unsigned char> lut(256, 0);
    for (int i = 0; i < 256; ++i) {
        float mapped = mapFn(cdf[i]);
        int rounded = static_cast<int>(std::round(mapped));
        lut[i] = static_cast<unsigned char>(clampv(rounded, 0, 255));
    }
    return lut;
}

CImg<unsigned char> apply_histogram_mapping(const CImg<unsigned char>& src,
                                            const function<float(float)>& mapFn) {
    const int w = src.width();
    const int h = src.height();
    const int s = src.spectrum();
    const int totalPixels = max(1, w * h);

    CImg<unsigned char> out(w, h, 1, s);
    for (int c = 0; c < s; ++c) {
        auto hist = compute_histogram(src, c);
        auto cdf = build_cdf(hist, totalPixels);
        auto lut = build_lut(cdf, mapFn);
        cimg_forXY(src, x, y) {
            out(x, y, c) = lut[src(x, y, c)];
        }
    }
    return out;
}

}

vector<int> compute_histogram(const CImg<unsigned char>& src, int channel) {
    if (channel < 0 || channel >= src.spectrum()) {
        throw runtime_error("compute_histogram channel index out of range");
    }
    vector<int> hist(256, 0);
    cimg_forXY(src, x, y) {
        hist[src(x, y, channel)]++;
    }
    return hist;
}

CImg<unsigned char> histogram_power23(const CImg<unsigned char>& src,
                                       int gmin,
                                       int gmax) {
    validate_range(gmin, gmax);
    const float gminThird = std::pow(static_cast<float>(gmin), 1.0f / 3.0f);
    const float gmaxThird = std::pow(static_cast<float>(gmax), 1.0f / 3.0f);
    return apply_histogram_mapping(src, [=](float cdfValue) {
        float gThird = gminThird + (gmaxThird - gminThird) * cdfValue;
        return std::pow(gThird, 3.0f);
    });
}

ImageCharacteristics compute_characteristics(const CImg<unsigned char>& src,
                                             int channel) {
    auto hist = compute_histogram(src, channel);
    const int N = max(1, src.width() * src.height());
    ImageCharacteristics ch{};

    ch.mean = 0.0;
    for (int m = 0; m < 256; ++m) {
        ch.mean += static_cast<double>(m) * hist[m];
    }
    ch.mean /= N;

    ch.variance = 0.0;
    for (int m = 0; m < 256; ++m) {
        double diff = static_cast<double>(m) - ch.mean;
        ch.variance += diff * diff * hist[m];
    }
    ch.variance /= N;

    ch.stdev = std::sqrt(ch.variance);
    ch.varcoeff_I = (ch.mean > 0.0) ? ch.stdev / ch.mean : 0.0;

    ch.asymmetry = 0.0;
    if (ch.stdev > 0.0) {
        double denom = static_cast<double>(N) * ch.stdev * ch.stdev * ch.stdev;
        for (int m = 0; m < 256; ++m) {
            double diff = static_cast<double>(m) - ch.mean;
            ch.asymmetry += diff * diff * diff * hist[m];
        }
        ch.asymmetry /= denom;
    }

    ch.flattening = 0.0;
    if (ch.stdev > 0.0) {
        double accum = 0.0;
        for (int m = 0; m < 256; ++m) {
            double diff = static_cast<double>(m) - ch.mean;
            accum += diff * diff * diff * diff * hist[m];
        }
        double denom = static_cast<double>(N) * ch.variance * ch.variance;
        if (denom > 0.0) {
            ch.flattening = accum / denom - 3.0;
        }
    }

    ch.varcoeff_II = 0.0;
    for (int m = 0; m < 256; ++m) {
        double p = static_cast<double>(hist[m]) / N;
        ch.varcoeff_II += p * p;
    }

    ch.entropy = 0.0;
    for (int m = 0; m < 256; ++m) {
        if (hist[m] > 0) {
            double p = static_cast<double>(hist[m]) / N;
            ch.entropy -= p * std::log2(p);
        }
    }

    return ch;
}
