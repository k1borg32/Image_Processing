#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include "Utils.h"
#include <string>

using namespace cimg_library;
using namespace std;

class ImageProcessor {
private:
    CImg<unsigned char> image;
    string inputPath;
    string outputPath;
    bool imageLoaded;

public:
    ImageProcessor();
    ~ImageProcessor();

    bool loadImage(const string& path);
    bool saveImage(const string& path);
    bool isLoaded() const { return imageLoaded; }

    int getWidth() const;
    int getHeight() const;
    int getChannels() const;

    void applyBrightness(int value);
    void applyContrast(float factor);
    void applyNegative();
    void applyRGBOffset(int rAdd, int gAdd, int bAdd);

    void applyHorizontalFlip();
    void applyVerticalFlip();
    void applyDiagonalFlip();
    void applyShrink(float factor);
    void applyEnlarge(float factor);

    void applyArithmeticMean(int kernelSize);
    void applyAdaptiveMedian(int kernelSize, int smax);

    const CImg<unsigned char>& getImage() const { return image; }
    
    void setImage(const CImg<unsigned char>& img);
};

#endif
