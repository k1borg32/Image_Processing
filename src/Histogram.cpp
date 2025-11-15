#include "Histogram.h"
#include <iostream>
#include <iomanip>

using namespace std;

vector<int> compute_histogram(const CImg<unsigned char>& src, int channel) {
    vector<int> hist(256, 0);
    cimg_forXY(src, x, y) {
        hist[src(x, y, channel)]++;
    }
    return hist;
}

CImg<unsigned char> histogram_power23(const CImg<unsigned char>& src, 
                                       int gmin, int gmax) {
    int w = src.width(), h = src.height(), s = src.spectrum();
    int N = w * h;
    CImg<unsigned char> out(w, h, 1, s);
    
    for (int c = 0; c < s; ++c) {
        auto hist = compute_histogram(src, c);
        
        // Build CDF lookup table
        vector<float> cdf(256);
        cdf[0] = (float)hist[0] / N;
        for (int i = 1; i < 256; ++i) {
            cdf[i] = cdf[i-1] + (float)hist[i] / N;
        }
        
        // Apply power 2/3 transformation
        float gmin_third = pow((float)gmin, 1.0f/3.0f);
        float gmax_third = pow((float)gmax, 1.0f/3.0f);
        
        cimg_forXY(src, x, y) {
            int f = src(x, y, c);
            float g_third = gmin_third + (gmax_third - gmin_third) * cdf[f];
            int g = (int)round(pow(g_third, 3.0f));
            out(x, y, c) = (unsigned char)clampv(g, 0, 255);
        }
    }
    return out;
}

void save_histogram_image(const vector<int>& hist, const string& outputPath) {
    int width = 512, height = 300;
    CImg<unsigned char> img(width, height, 1, 1, 255);
    
    // Find max value for scaling
    int maxVal = *max_element(hist.begin(), hist.end());
    if (maxVal == 0) maxVal = 1;
    
    // Draw histogram bars
    for (int i = 0; i < 256; ++i) {
        int barHeight = (int)((float)hist[i] / maxVal * (height - 20));
        int x = i * 2;
        for (int y = height - barHeight; y < height; ++y) {
            img(x, y) = 0;
            if (x + 1 < width) img(x + 1, y) = 0;
        }
    }
    
    img.save(outputPath.c_str());
    cout << "Histogram saved to: " << outputPath << endl;
}

ImageCharacteristics compute_characteristics(const CImg<unsigned char>& src, 
                                             int channel) {
    auto hist = compute_histogram(src, channel);
    int N = src.width() * src.height();
    ImageCharacteristics ch;
    
    // C1: Mean
    ch.mean = 0.0;
    for (int m = 0; m < 256; ++m) {
        ch.mean += m * hist[m];
    }
    ch.mean /= N;
    
    // C1: Variance
    ch.variance = 0.0;
    for (int m = 0; m < 256; ++m) {
        double diff = m - ch.mean;
        ch.variance += diff * diff * hist[m];
    }
    ch.variance /= N;
    
    // C2: Standard deviation
    ch.stdev = sqrt(ch.variance);
    
    // C2: Variation coefficient I
    ch.varcoeff_I = (ch.mean > 0) ? ch.stdev / ch.mean : 0.0;
    
    // C3: Asymmetry coefficient
    ch.asymmetry = 0.0;
    if (ch.stdev > 0) {
        for (int m = 0; m < 256; ++m) {
            double diff = m - ch.mean;
            ch.asymmetry += diff * diff * diff * hist[m];
        }
        ch.asymmetry /= (N * ch.stdev * ch.stdev * ch.stdev);
    }
    
    // C4: Flattening coefficient
    ch.flattening = 0.0;
    if (ch.stdev > 0) {
        for (int m = 0; m < 256; ++m) {
            double diff = m - ch.mean;
            ch.flattening += diff * diff * diff * diff * hist[m];
        }
        ch.flattening = ch.flattening / (N * ch.variance * ch.variance) - 3.0;
    }
    
    // C5: Variation coefficient II - FIXED
    ch.varcoeff_II = 0.0;
    for (int m = 0; m < 256; ++m) {
        double h_normalized = (double)hist[m] / N;
        ch.varcoeff_II += h_normalized * h_normalized;
    }
    
    // C6: Entropy
    ch.entropy = 0.0;
    for (int m = 0; m < 256; ++m) {
        if (hist[m] > 0) {
            double p = (double)hist[m] / N;
            ch.entropy -= p * log2(p);
        }
    }
    
    return ch;
}
