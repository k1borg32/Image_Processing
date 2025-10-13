#include <iostream>
#include <string>

#define cimg_display 0
#define cimg_use_bmp
#include "CImg.h"

using namespace std;
using namespace cimg_library;


void doBrightness(string param, string inputFile, string outputFile) {
    cout << "Function doBrightness invoked with param: " << param << endl;
    
    try {
        CImg<unsigned char> image(inputFile.c_str());
        cout << "Loaded image: " << inputFile << " (" << image.width() << "x" << image.height() << ")" << endl;
        
        int brightness = stoi(param);
        cout << "Adjusting brightness by: " << brightness << endl;
        
        for (int x = 0; x < image.width(); x++) {
            for (int y = 0; y < image.height(); y++) {
                for (int c = 0; c < image.spectrum(); c++) {
                    int pixel = image(x, y, c);
                    pixel += brightness;
                    if (pixel < 0) pixel = 0;
                    if (pixel > 255) pixel = 255;
                    image(x, y, c) = pixel;
                }
            }
        }
        
        image.save_bmp(outputFile.c_str());
        cout << "Saved processed image to: " << outputFile << endl;
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void doContrast(string param, string inputFile, string outputFile) {
    cout << "Function doContrast invoked with param: " << param << endl;
    
    try {
        CImg<unsigned char> image(inputFile.c_str());
        cout << "Loaded image: " << inputFile << " (" << image.width() << "x" << image.height() << ")" << endl;
        
        float contrast = stof(param);
        cout << "Adjusting contrast by factor: " << contrast << endl;
        
        for (int x = 0; x < image.width(); x++) {
            for (int y = 0; y < image.height(); y++) {
                for (int c = 0; c < image.spectrum(); c++) {
                    int pixel = image(x, y, c);
                    int newPixel = (int)((pixel - 128.0f) * contrast + 128.0f);
                    if (newPixel < 0) newPixel = 0;
                    if (newPixel > 255) newPixel = 255;
                    image(x, y, c) = newPixel;
                }
            }
        }
        
        image.save_bmp(outputFile.c_str());
        cout << "Saved processed image to: " << outputFile << endl;
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void doGrayscale(string inputFile, string outputFile) {
    cout << "Function doGrayscale invoked" << endl;
    
    try {
        CImg<unsigned char> image(inputFile.c_str());
        cout << "Loaded image: " << inputFile << " (" << image.width() << "x" << image.height() << ")" << endl;
        
        for (int x = 0; x < image.width(); x++) {
            for (int y = 0; y < image.height(); y++) {
                if (image.spectrum() >= 3) {
                    float r = image(x, y, 0);
                    float g = image(x, y, 1);
                    float b = image(x, y, 2);
                    
                    int gray = (int)(0.299f * r + 0.587f * g + 0.114f * b);
                    
                    image(x, y, 0) = gray;
                    image(x, y, 1) = gray;
                    image(x, y, 2) = gray;
                }
            }
        }
        
        image.save_bmp(outputFile.c_str());
        cout << "Saved processed image to: " << outputFile << endl;
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}


int main(int argc, char* argv[]) {
    cout << "\n\nImage Processing Application" << endl;
    cout << "Uses CImg library for BMP file I/O\n" << endl;

    if (argc == 1) {
        cout << "No command line parameters given.\n" << endl;
        cout << "Usage: " << argv[0] << " <command> [parameter] <input.bmp> [output.bmp]" << endl;
        cout << "Commands:" << endl;
        cout << "  --brightness <value>   Adjust brightness (-255 to 255)" << endl;
        cout << "  --contrast <factor>    Adjust contrast (0.1 to 3.0)" << endl;
        cout << "  --grayscale           Convert to grayscale" << endl;
        return 0;
    }

    if (argc < 3) {
        cout << "Too few command line parameters given.\n" << endl;
        return 0;
    }

    string command = argv[1];
    
    if (command == "--brightness") {
        if (argc < 4) {
            cout << "Error: --brightness requires a parameter and input file" << endl;
            return 0;
        }
        string param = argv[2];
        string inputFile = argv[3];
        string outputFile = (argc > 4) ? argv[4] : "output.bmp";
        doBrightness(param, inputFile, outputFile);
        
    } else if (command == "--contrast") {
        if (argc < 4) {
            cout << "Error: --contrast requires a parameter and input file" << endl;
            return 0;
        }
        string param = argv[2];
        string inputFile = argv[3];
        string outputFile = (argc > 4) ? argv[4] : "output.bmp";
        doContrast(param, inputFile, outputFile);
        
    } else if (command == "--grayscale") {
        string inputFile = argv[2];
        string outputFile = (argc > 3) ? argv[3] : "output.bmp";
        doGrayscale(inputFile, outputFile);
        
    } else {
        cout << "Unknown command: " + command << endl;
        cout << "Available commands: --brightness, --contrast, --grayscale" << endl;
    }

    cout << endl;
    return 0;
}