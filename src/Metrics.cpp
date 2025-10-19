#include "Metrics.h"
#include <fstream>

using namespace std;
using namespace cimg_library;

PairImages load_pair(const string& refPath, const string& cmpPath) {
    string absRef = resolveAbsolutePath(refPath);
    string absCmp = resolveAbsolutePath(cmpPath);
    
    CImg<unsigned char> ref, cmp;
    ref.load_bmp(absRef.c_str());
    cmp.load_bmp(absCmp.c_str());
    
    PairImages p { ref, cmp };
    if (p.ref.width() != p.cmp.width() || p.ref.height() != p.cmp.height() || p.ref.spectrum() != p.cmp.spectrum())
        throw runtime_error("Images must have identical dimensions and channels for comparison");
    return p;
}

double metric_MSE(const PairImages& P) {
    const auto& A = P.ref; const auto& B = P.cmp;
    double sum = 0.0;
    cimg_forXY(A, x, y) for (int c = 0; c < A.spectrum(); ++c) {
        double d = (double)A(x, y, c) - (double)B(x, y, c);
        sum += d * d;
    }
    double denom = (double)A.width() * A.height() * A.spectrum();
    return sum / denom;
}

double metric_PMSE(const PairImages& P) {
    const auto& A = P.ref; const auto& B = P.cmp;
    unsigned char maxv = 0;
    cimg_forXY(A, x, y) for (int c = 0; c < A.spectrum(); ++c) maxv = max(maxv, A(x, y, c));
    double denomMax2 = (double)maxv * (double)maxv;
    if (denomMax2 <= 0.0) return numeric_limits<double>::infinity();
    double sum = 0.0;
    cimg_forXY(A, x, y) for (int c = 0; c < A.spectrum(); ++c) {
        double d = (double)A(x, y, c) - (double)B(x, y, c);
        sum += (d * d) / denomMax2;
    }
    double denom = (double)A.width() * A.height() * A.spectrum();
    return sum / denom;
}

double metric_SNR_dB(const PairImages& P) {
    const auto& A = P.ref; const auto& B = P.cmp;
    double signal = 0.0, noise = 0.0;
    cimg_forXY(A, x, y) for (int c = 0; c < A.spectrum(); ++c) {
        double af = (double)A(x, y, c);
        double bf = (double)B(x, y, c);
        signal += af * af;
        double d = af - bf; noise += d * d;
    }
    if (noise <= 1e-12) return numeric_limits<double>::infinity();
    return 10.0 * log10(signal / noise);
}

double metric_PSNR_dB(const PairImages& P) {
    const auto& A = P.ref; const auto& B = P.cmp;
    unsigned char maxv = 0;
    cimg_forXY(A, x, y) for (int c = 0; c < A.spectrum(); ++c) maxv = max(maxv, A(x, y, c));
    double max2sum = (double)A.width() * A.height() * A.spectrum() * (double)maxv * (double)maxv;
    double errsum  = 0.0;
    cimg_forXY(A, x, y) for (int c = 0; c < A.spectrum(); ++c) {
        double d = (double)A(x, y, c) - (double)B(x, y, c);
        errsum += d * d;
    }
    if (errsum <= 1e-12) return numeric_limits<double>::infinity();
    return 10.0 * log10(max2sum / errsum);
}

int metric_MD(const PairImages& P) {
    const auto& A = P.ref; const auto& B = P.cmp;
    int md = 0;
    cimg_forXY(A, x, y) for (int c = 0; c < A.spectrum(); ++c) {
        int d = abs((int)A(x, y, c) - (int)B(x, y, c));
        if (d > md) md = d;
    }
    return md;
}
