#ifndef METRICS_H
#define METRICS_H
#include "Utils.h"

using namespace cimg_library;
using namespace std;

struct PairImages {
    CImg<unsigned char> ref;
    CImg<unsigned char> cmp;
};

PairImages load_pair(const string& refPath, const string& cmpPath);

double metric_MSE (const PairImages& P);
double metric_PMSE(const PairImages& P);
double metric_SNR_dB (const PairImages& P);
double metric_PSNR_dB(const PairImages& P);
int    metric_MD    (const PairImages& P);

#endif 
