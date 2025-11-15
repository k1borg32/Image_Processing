#include "Histogram.h"
#include "LinearFilters.h"
#include "NonLinearFilters.h"
#include <iomanip> 
#include <iostream>
#include <string>

using namespace std;

void printHelp() {
    cout << "Image Processing - Task 2\n";
    cout << "Usage: ./imageProcessor --command -input=file -output=file [options]\n\n";
    cout << "Commands:\n";
    cout << "  --hpower         : Apply H4 power 2/3 histogram equalization\n";
    cout << "  --histogram      : Save histogram as image\n";
    cout << "  --characteristics: Print all image characteristics (C1-C6)\n";
    cout << "  --sedgesharp     : Apply edge sharpening (S2)\n";
    cout << "                     Options: -variant=1,2,3 or -optimized\n";
    cout << "  --orosenfeld     : Apply Rosenfeld operator (O5)\n";
    cout << "                     Options: -P=1,2,4,8,16\n";
    cout << "\nOptions:\n";
    cout << "  -input=PATH      : Input image file\n";
    cout << "  -output=PATH     : Output image file\n";
    cout << "  -channel=N       : Channel for histogram (0,1,2)\n";
    cout << "  -gmin=N          : Min value for histogram (default: 0)\n";
    cout << "  -gmax=N          : Max value for histogram (default: 255)\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 0;
    }
    
    string command, inputPath, outputPath;
    int channel = 0, gmin = 0, gmax = 255, variant = 1, P = 1;
    bool optimized = false;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return 0;
        }
        else if (arg.find("--") == 0) {
            command = arg;
        }
        else if (arg.find("-input=") == 0) {
            inputPath = arg.substr(7);
        }
        else if (arg.find("-output=") == 0) {
            outputPath = arg.substr(8);
        }
        else if (arg.find("-channel=") == 0) {
            channel = stoi(arg.substr(9));
        }
        else if (arg.find("-gmin=") == 0) {
            gmin = stoi(arg.substr(6));
        }
        else if (arg.find("-gmax=") == 0) {
            gmax = stoi(arg.substr(6));
        }
        else if (arg.find("-variant=") == 0) {
            variant = stoi(arg.substr(9));
        }
        else if (arg.find("-P=") == 0) {
            P = stoi(arg.substr(3));
        }
        else if (arg == "-optimized") {
            optimized = true;
        }
    }
    
    if (inputPath.empty()) {
        cerr << "Error: No input file specified\n";
        return 1;
    }
    
    try {
        // Load image
        CImg<unsigned char> img(inputPath.c_str());
        cout << "Loaded: " << inputPath << " (" << img.width() << "x" 
             << img.height() << ", " << img.spectrum() << " channels)\n";
        
        CImg<unsigned char> result;
        
        // Process based on command
        if (command == "--hpower") {
            result = histogram_power23(img, gmin, gmax);
            cout << "Applied power 2/3 histogram equalization\n";
        }
        else if (command == "--histogram") {
            auto hist = compute_histogram(img, channel);
            save_histogram_image(hist, outputPath);
            return 0;
        }
        else if (command == "--characteristics") {
            auto ch = compute_characteristics(img, channel);
            cout << fixed << setprecision(4);
            cout << "\nImage Characteristics (Channel " << channel << "):\n";
            cout << "  Mean (C1):              " << ch.mean << "\n";
            cout << "  Variance (C1):          " << ch.variance << "\n";
            cout << "  Std Deviation (C2):     " << ch.stdev << "\n";
            cout << "  Var Coefficient I (C2): " << ch.varcoeff_I << "\n";
            cout << "  Asymmetry (C3):         " << ch.asymmetry << "\n";
            cout << "  Flattening (C4):        " << ch.flattening << "\n";
            cout << "  Var Coefficient II (C5):" << ch.varcoeff_II << "\n";
            cout << "  Entropy (C6):           " << ch.entropy << " bits\n";
            return 0;
        }
        else if (command == "--sedgesharp") {
            if (optimized) {
                result = edge_sharpen_optimized(img);
                cout << "Applied optimized edge sharpening\n";
            } else {
                if (variant == 1) result = edge_sharpen_type1(img);
                else if (variant == 2) result = edge_sharpen_type2(img);
                else if (variant == 3) result = edge_sharpen_type3(img);
                cout << "Applied edge sharpening (variant " << variant << ")\n";
            }
        }
        else if (command == "--orosenfeld") {
            result = rosenfeld_operator(img, P);
            cout << "Applied Rosenfeld operator (P=" << P << ")\n";
        }
        else {
            cerr << "Unknown command: " << command << "\n";
            return 1;
        }
        
        // Save result
        if (outputPath.empty()) {
            outputPath = "output.bmp";
        }
        result.save(outputPath.c_str());
        cout << "Saved: " << outputPath << "\n";
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
