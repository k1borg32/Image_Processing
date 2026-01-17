#include "CImg.h"
#include "FFT.h"
#include "FrequencyFilters.h"
#include <iostream>
#include <string>
#include <cstring>

using namespace cimg_library;

void printHelp() {
    std::cout << "Task 4: Frequency Domain Filtering (FFT)\n";
    std::cout << "Usage: ./build/task4 --filter -input=file -output=file [options]\n\n";
    std::cout << "Filters:\n";
    std::cout << "  -type=lowpass   (F1) Low-pass filter\n";
    std::cout << "    -R=value      Cut-off radius\n";
    std::cout << "  -type=highpass  (F2) High-pass filter\n";
    std::cout << "    -R=value      Cut-off radius\n";
    std::cout << "  -type=bandpass  (F3) Band-pass filter\n";
    std::cout << "    -Rmin=value   Inner radius\n";
    std::cout << "    -Rmax=value   Outer radius\n";
    std::cout << "  -type=bandcut   (F4) Band-cut filter\n";
    std::cout << "    -Rmin=value   Inner radius\n";
    std::cout << "    -Rmax=value   Outer radius\n";
    std::cout << "  -type=hpdir     (F5) High-pass with direction\n";
    std::cout << "    -R=value      Radius\n";
    std::cout << "    -theta=value  Direction angle (degrees)\n";
    std::cout << "    -phi=value    Wedge width (degrees)\n";
    std::cout << "  -type=phase     (F6) Phase modifying filter\n";
    std::cout << "    -k=value      Shift in x direction\n";
    std::cout << "    -l=value      Shift in y direction\n";
    std::cout << "\nOther options:\n";
    std::cout << "  -spectrum=file  Save magnitude spectrum image\n";
    std::cout << "  -mask=file      Save filter mask image\n";
    std::cout << "  --help          Show this help\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 0;
    }
    
    std::string inputPath, outputPath, spectrumPath, maskPath;
    std::string filterType = "lowpass";
    double R = 50, Rmin = 20, Rmax = 80;
    double theta = 0, phi = 30;
    int k = 0, l = 0;
    
    // parse args
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help") { printHelp(); return 0; }
        else if (arg.find("-input=") == 0) inputPath = arg.substr(7);
        else if (arg.find("-output=") == 0) outputPath = arg.substr(8);
        else if (arg.find("-spectrum=") == 0) spectrumPath = arg.substr(10);
        else if (arg.find("-mask=") == 0) maskPath = arg.substr(6);
        else if (arg.find("-type=") == 0) filterType = arg.substr(6);
        else if (arg.find("-R=") == 0) R = std::stod(arg.substr(3));
        else if (arg.find("-Rmin=") == 0) Rmin = std::stod(arg.substr(6));
        else if (arg.find("-Rmax=") == 0) Rmax = std::stod(arg.substr(6));
        else if (arg.find("-theta=") == 0) theta = std::stod(arg.substr(7));
        else if (arg.find("-phi=") == 0) phi = std::stod(arg.substr(5));
        else if (arg.find("-k=") == 0) k = std::stoi(arg.substr(3));
        else if (arg.find("-l=") == 0) l = std::stoi(arg.substr(3));
    }
    
    if (inputPath.empty()) {
        std::cerr << "Error: No input file specified.\n";
        return 1;
    }
    if (outputPath.empty()) {
        outputPath = "output_images/filtered.bmp";
    }
    
    try {
        // load image and convert to grayscale
        CImg<unsigned char> img(inputPath.c_str());
        if (img.spectrum() > 1) {
            // convert to grayscale: 0.299*R + 0.587*G + 0.114*B
            CImg<unsigned char> gray(img.width(), img.height(), 1, 1);
            cimg_forXY(img, x, y) {
                double val = 0.299 * img(x, y, 0) + 0.587 * img(x, y, 1) + 0.114 * img(x, y, 2);
                gray(x, y) = (unsigned char)val;
            }
            img = gray;
        }
        
        int width = img.width();
        int height = img.height();
        
        std::cout << "[FFT] Image size: " << width << "x" << height << std::endl;
        
        // pad to power of 2 for FFT
        size_t N = next_power_of_2(height);
        size_t M = next_power_of_2(width);
        std::cout << "[FFT] Padded size: " << M << "x" << N << std::endl;
        
        // convert image to complex matrix
        ComplexMatrix imgComplex(N, ComplexVector(M, Complex(0, 0)));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                imgComplex[y][x] = Complex(img(x, y), 0);
            }
        }
        
        // forward FFT
        std::cout << "[FFT] Computing 2D FFT..." << std::endl;
        ComplexMatrix spectrum = fft_2d(imgComplex);
        
        // shift so DC is at center
        ComplexMatrix spectrumShifted = fft_shift(spectrum);
        
        // save magnitude spectrum if requested
        if (!spectrumPath.empty()) {
            std::vector<double> magSpec(N * M);
            get_magnitude_spectrum(spectrumShifted, magSpec.data(), M, N, true);
            
            CImg<unsigned char> specImg(M, N, 1, 1);
            for (size_t y = 0; y < N; y++) {
                for (size_t x = 0; x < M; x++) {
                    specImg(x, y) = (unsigned char)magSpec[y * M + x];
                }
            }
            specImg.save(spectrumPath.c_str());
            std::cout << "[FFT] Spectrum saved to " << spectrumPath << std::endl;
        }
        
        // create filter mask based on type
        std::cout << "[FFT] Creating " << filterType << " filter..." << std::endl;
        ComplexMatrix mask;
        bool isPhaseFilter = false;
        
        if (filterType == "lowpass") {
            mask = filter_lowpass(M, N, R);
            std::cout << "[FFT] Low-pass with R=" << R << std::endl;
        }
        else if (filterType == "highpass") {
            mask = filter_highpass(M, N, R);
            std::cout << "[FFT] High-pass with R=" << R << std::endl;
        }
        else if (filterType == "bandpass") {
            mask = filter_bandpass(M, N, Rmin, Rmax);
            std::cout << "[FFT] Band-pass with Rmin=" << Rmin << ", Rmax=" << Rmax << std::endl;
        }
        else if (filterType == "bandcut") {
            mask = filter_bandcut(M, N, Rmin, Rmax);
            std::cout << "[FFT] Band-cut with Rmin=" << Rmin << ", Rmax=" << Rmax << std::endl;
        }
        else if (filterType == "hpdir") {
            mask = filter_highpass_direction(M, N, R, theta, phi);
            std::cout << "[FFT] High-pass directional with R=" << R << ", theta=" << theta << ", phi=" << phi << std::endl;
        }
        else if (filterType == "phase") {
            mask = filter_phase_modify(M, N, k, l);
            isPhaseFilter = true;
            std::cout << "[FFT] Phase shift with k=" << k << ", l=" << l << std::endl;
        }
        else {
            std::cerr << "Unknown filter type: " << filterType << std::endl;
            return 1;
        }
        
        // save mask if requested (only for non-phase filters)
        if (!maskPath.empty() && !isPhaseFilter) {
            CImg<unsigned char> maskImg(M, N, 1, 1);
            for (size_t y = 0; y < N; y++) {
                for (size_t x = 0; x < M; x++) {
                    maskImg(x, y) = (unsigned char)(mask[y][x].real() * 255);
                }
            }
            maskImg.save(maskPath.c_str());
            std::cout << "[FFT] Mask saved to " << maskPath << std::endl;
        }
        
        // apply filter
        std::cout << "[FFT] Applying filter..." << std::endl;
        ComplexMatrix filtered = apply_filter(spectrumShifted, mask);
        
        // shift back
        ComplexMatrix filteredUnshifted = fft_shift(filtered);
        
        // inverse FFT
        std::cout << "[FFT] Computing inverse FFT..." << std::endl;
        ComplexMatrix result = ifft_2d(filteredUnshifted);
        
        // extract real part and save
        CImg<unsigned char> output(width, height, 1, 1);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double val = result[y][x].real();
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                output(x, y) = (unsigned char)(val + 0.5);
            }
        }
        
        output.save(outputPath.c_str());
        std::cout << "[FFT] Result saved to " << outputPath << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
