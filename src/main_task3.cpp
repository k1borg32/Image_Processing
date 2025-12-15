#include "Utils.h"
#include "Morphology.h"
#include "Segmentation.h"
#include <iostream>
#include <string>

CImg<unsigned char> binarize(const CImg<unsigned char>& img, int threshold = 127) {
    CImg<unsigned char> result(img.width(), img.height(), 1, 1, 0);
    cimg_forXY(img, x, y) {
        result(x, y) = (img(x, y) > threshold) ? 255 : 0;
    }
    return result;
}

void printHelp() {
    std::cout << "Task 3: Morphology and Segmentation\n";
    std::cout << "Usage: ./build/task3 --command -input=file [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  --morph         : Apply morphological operations\n";
    std::cout << "    -op=NAME      : dilate, erode, open, close, thin\n";
    std::cout << "    -elem=TYPE    : square (default), cross\n";
    std::cout << "  --segment       : Apply Region Growing segmentation (R1)\n";
    std::cout << "    -seed=X,Y     : Seed point coordinates (e.g., -seed=100,100)\n";
    std::cout << "    -thresh=VAL   : Color distance threshold (default: 40)\n";
    std::cout << "  --help          : Show this help\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 0;
    }

    std::string command;
    std::string inputPath; 
    std::string outputPath = "output_images/out.bmp";
    
    std::string morphOp = "dilate";
    std::string structElem = "square";
    int seedX = 0, seedY = 0;
    int segThresh = 40;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") { printHelp(); return 0; }
        else if (arg.find("--") == 0) command = arg;
        else if (arg.find("-input=") == 0) inputPath = arg.substr(7);
        else if (arg.find("-output=") == 0) outputPath = arg.substr(8);
        else if (arg.find("-op=") == 0) morphOp = arg.substr(4);
        else if (arg.find("-elem=") == 0) structElem = arg.substr(6);
        else if (arg.find("-seed=") == 0) {
            std::string coords = arg.substr(6);
            size_t comma = coords.find(',');
            if (comma != std::string::npos) {
                seedX = std::stoi(coords.substr(0, comma));
                seedY = std::stoi(coords.substr(comma + 1));
            }
        }
        else if (arg.find("-thresh=") == 0) segThresh = std::stoi(arg.substr(8));
    }

    if (inputPath.empty()) {
        std::cerr << "Error: No input file specified.\n";
        return 1;
    }

    // Normalize path for Windows
    std::string normalizedInput = inputPath;
    for (char& c : normalizedInput) {
        if (c == '\\') c = '/';
    }

    try {
        CImg<unsigned char> img(normalizedInput.c_str());
        CImg<unsigned char> result;

        if (command == "--morph") {
            CImg<unsigned char> bin = binarize(img);
            
            StructElement se = (structElem == "cross") ? 
                               StructElement::createCross3x3() : 
                               StructElement::createSquare3x3();
            
            if (morphOp == "dilate") result = morph_dilation(bin, se);
            else if (morphOp == "erode") result = morph_erosion(bin, se);
            else if (morphOp == "open") result = morph_opening(bin, se);
            else if (morphOp == "close") result = morph_closing(bin, se);
            else if (morphOp == "thin") result = morph_thinning(bin);
            else {
                std::cerr << "Unknown op: " << morphOp << std::endl;
                return 1;
            }
        }
        else if (command == "--segment") {
            CImg<unsigned char> mask = region_growing(img, seedX, seedY, segThresh);
            
            result = img;
            if (result.spectrum() == 1) result.resize(result.width(), result.height(), 1, 3);
            
            cimg_forXY(mask, x, y) {
                if (mask(x, y) == 255) {
                    result(x, y, 0) = 255; 
                    result(x, y, 1) = 0;   
                    result(x, y, 2) = 0;
                }
            }
        }
        else {
            std::cerr << "Unknown command. Use --morph or --segment.\n";
            return 1;
        }

        result.save(outputPath.c_str());
        std::cout << "Saved to " << outputPath << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
