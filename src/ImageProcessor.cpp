#include "ImageProcessor.h"
#include "Operations.h"
#include "Geometric.h"
#include "NoiseFilters.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cimg_library;

ImageProcessor::ImageProcessor() : imageLoaded(false) {}

ImageProcessor::~ImageProcessor() {}

bool ImageProcessor::loadImage(const string& path) {
    try {
        string absPath = resolveAbsolutePath(path);
        ifstream testFile(absPath.c_str(), ios::binary);
        if (!testFile.good()) {
            cerr << "[ImageProcessor] File not found: " << absPath << endl;
            return false;
        }
        testFile.close();
        
        FILE* file = fopen(absPath.c_str(), "rb");
        if (!file) {
            cerr << "[ImageProcessor] Cannot open file: " << absPath << endl;
            return false;
        }
        fclose(file);
        
        image.assign();
        image.load_bmp(absPath.c_str());
        
        inputPath = path;
        imageLoaded = true;
        cout << "[ImageProcessor] Loaded: " << path 
             << " (" << image.width() << "x" << image.height() 
             << "x" << image.spectrum() << " channels)" << endl;
        return true;
    } catch (const CImgException& e) {
        cerr << "[ImageProcessor] Error loading image: " << e.what() << endl;
        imageLoaded = false;
        return false;
    } catch (const exception& e) {
        cerr << "[ImageProcessor] Error: " << e.what() << endl;
        imageLoaded = false;
        return false;
    }
}

bool ImageProcessor::saveImage(const string& path) {
    if (!imageLoaded) {
        cerr << "[ImageProcessor] No image loaded to save!" << endl;
        return false;
    }
    try {
        saveAsBMP(image, path);
        outputPath = path;
        cout << "[ImageProcessor] Saved to: " << path << endl;
        return true;
    } catch (const CImgException& e) {
        cerr << "[ImageProcessor] Error saving image: " << e.what() << endl;
        return false;
    }
}

int ImageProcessor::getWidth() const {
    return imageLoaded ? image.width() : 0;
}

int ImageProcessor::getHeight() const {
    return imageLoaded ? image.height() : 0;
}

int ImageProcessor::getChannels() const {
    return imageLoaded ? image.spectrum() : 0;
}

void ImageProcessor::setImage(const CImg<unsigned char>& img) {
    image = img;
    imageLoaded = true;
}

void ImageProcessor::applyBrightness(int value) {
    if (!imageLoaded) throw runtime_error("No image loaded");
    if (value < -255 || value > 255) throw runtime_error("Brightness value must be in [-255, 255]");
    cout << "[ImageProcessor] Applying brightness: " << value << endl;
    image = op_brightness(image, value);
}

void ImageProcessor::applyContrast(float factor) {
    if (!imageLoaded) throw runtime_error("No image loaded");
    if (factor < 0.1f || factor > 3.0f) throw runtime_error("Contrast factor must be in [0.1, 3.0]");
    cout << "[ImageProcessor] Applying contrast: " << factor << endl;
    image = op_contrast_linear(image, factor);
}

void ImageProcessor::applyNegative() {
    if (!imageLoaded) throw runtime_error("No image loaded");
    cout << "[ImageProcessor] Applying negative filter" << endl;
    image = op_negative(image);
}

void ImageProcessor::applyRGBOffset(int rAdd, int gAdd, int bAdd) {
    if (!imageLoaded) throw runtime_error("No image loaded");
    cout << "[ImageProcessor] Applying RGB offset: R=" << rAdd 
         << ", G=" << gAdd << ", B=" << bAdd << endl;
    image = op_rgb_add(image, rAdd, gAdd, bAdd);
}

void ImageProcessor::applyHorizontalFlip() {
    if (!imageLoaded) throw runtime_error("No image loaded");
    cout << "[ImageProcessor] Applying horizontal flip" << endl;
    image = op_hflip(image);
}

void ImageProcessor::applyVerticalFlip() {
    if (!imageLoaded) throw runtime_error("No image loaded");
    cout << "[ImageProcessor] Applying vertical flip" << endl;
    image = op_vflip(image);
}

void ImageProcessor::applyDiagonalFlip() {
    if (!imageLoaded) throw runtime_error("No image loaded");
    cout << "[ImageProcessor] Applying diagonal flip (transpose)" << endl;
    image = op_dflip(image);
}

void ImageProcessor::applyShrink(float factor) {
    if (!imageLoaded) throw runtime_error("No image loaded");
    if (factor <= 0.0f || factor >= 1.0f) throw runtime_error("Shrink factor must be in (0, 1)");
    cout << "[ImageProcessor] Shrinking image by factor: " << factor << endl;
    image = op_shrink(image, factor);
}

void ImageProcessor::applyEnlarge(float factor) {
    if (!imageLoaded) throw runtime_error("No image loaded");
    if (factor <= 1.0f) throw runtime_error("Enlarge factor must be > 1.0");
    cout << "[ImageProcessor] Enlarging image by factor: " << factor << endl;
    image = op_enlarge(image, factor);
}

void ImageProcessor::applyArithmeticMean(int kernelSize) {
    if (!imageLoaded) throw runtime_error("No image loaded");
    if (!isOdd(kernelSize) || kernelSize < 3) 
        throw runtime_error("Kernel size must be odd and >= 3");
    cout << "[ImageProcessor] Applying arithmetic mean filter, kernel=" << kernelSize << endl;
    image = op_amean(image, kernelSize);
}

void ImageProcessor::applyAdaptiveMedian(int kernelSize, int smax) {
    if (!imageLoaded) throw runtime_error("No image loaded");
    if (!isOdd(kernelSize) || kernelSize < 3) 
        throw runtime_error("Kernel size must be odd and >= 3");
    if (!isOdd(smax) || smax < kernelSize) 
        throw runtime_error("Smax must be odd and >= kernel size");
    cout << "[ImageProcessor] Applying adaptive median filter, kernel=" 
         << kernelSize << ", smax=" << smax << endl;
    image = op_adaptive_median(image, kernelSize, smax);
}
