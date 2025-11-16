#include "Histogram.h"
#include "LinearFilters.h"
#include "NonLinearFilters.h"
#include "Utils.h"
#include <cctype>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

void printHelp() {
    cout << "Image Processing - Task 2\n";
    cout << "Usage: ./imageProcessor --command -input=file -output=file [options]\n\n";
    cout << "Supported commands:\n";
    cout << "  --hpower         : Power 2/3 histogram equalisation (-gmin, -gmax)\n";
    cout << "  --characteristics : Print all image characteristics (-channel)\n";
    cout << "  --cmean, --cvariance, --cstdev, --cvarcoi\n";
    cout << "  --casyco, --cflatco, --cvarcoii, --centropy\n";
    cout << "  --sedgesharp     : Edge sharpening (-variant=1..3 or -optimized)\n";
    cout << "  --orosenfeld     : Rosenfeld operator (-P=1,2,4,8,16)\n\n";
    cout << "Options:\n";
    cout << "  -input=PATH      : Input image file\n";
    cout << "  -output=PATH     : Output image file\n";
    cout << "  -channel=N       : Channel index (0-based)\n";
    cout << "  -gmin=N          : Min output level (default 0)\n";
    cout << "  -gmax=N          : Max output level (default 255)\n";
    cout << "  -variant=N       : Variant for --sedgesharp (1..3)\n";
    cout << "  -optimized       : Use optimized sharpening\n";
    cout << "  -P=N             : Window size for Rosenfeld (1,2,4,8,16)\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 0;
    }
    
    string command, inputPath, outputPath;
    int channel = 0;
    int gmin = 0, gmax = 255;
    int variant = 1;
    int P = 1;
    bool optimized = false;
    
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
    
    if (command.empty()) {
        cerr << "Error: No command provided\n";
        return 1;
    }

    if (inputPath.empty()) {
        cerr << "Error: No input file specified\n";
        return 1;
    }
    
    try {
        CImg<unsigned char> img(inputPath.c_str());
        cout << "Loaded: " << inputPath << " (" << img.width() << "x" 
             << img.height() << ", " << img.spectrum() << " channels)\n";
        
        auto slugify = [](const string& cmd) {
            string slug = (cmd.rfind("--", 0) == 0) ? cmd.substr(2) : cmd;
            for (char& ch : slug) {
                unsigned char uch = static_cast<unsigned char>(ch);
                ch = std::isalnum(uch) ? ch : '_';
            }
            return slug;
        };

        auto ensureOutput = [&](const string& suffix) {
            if (outputPath.empty()) {
                outputPath = "output/" + suffix + ".bmp";
            }
        };

        auto ensureChannelValid = [&]() {
            if (channel < 0 || channel >= img.spectrum()) {
                throw runtime_error("Selected channel is out of range for this image");
            }
        };

        bool characteristicsComputed = false;
        ImageCharacteristics cachedCharacteristics;
        auto getCharacteristics = [&]() -> const ImageCharacteristics& {
            if (!characteristicsComputed) {
                ensureChannelValid();
                cachedCharacteristics = compute_characteristics(img, channel);
                characteristicsComputed = true;
            }
            return cachedCharacteristics;
        };

        CImg<unsigned char> result;
        bool produceImage = false;
        string actionDescription;

        if (command == "--hpower") {
            result = histogram_power23(img, gmin, gmax);
            produceImage = true;
            actionDescription = "Applied power 2/3 histogram equalisation";
            ensureOutput(slugify(command));
        }
        else if (command == "--characteristics") {
            const auto& ch = getCharacteristics();
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
        else if (command == "--cmean") {
            const auto& ch = getCharacteristics();
            cout << fixed << setprecision(6) << ch.mean << "\n";
            return 0;
        }
        else if (command == "--cvariance") {
            const auto& ch = getCharacteristics();
            cout << fixed << setprecision(6) << ch.variance << "\n";
            return 0;
        }
        else if (command == "--cstdev") {
            const auto& ch = getCharacteristics();
            cout << fixed << setprecision(6) << ch.stdev << "\n";
            return 0;
        }
        else if (command == "--cvarcoi") {
            const auto& ch = getCharacteristics();
            cout << fixed << setprecision(6) << ch.varcoeff_I << "\n";
            return 0;
        }
        else if (command == "--casyco") {
            const auto& ch = getCharacteristics();
            cout << fixed << setprecision(6) << ch.asymmetry << "\n";
            return 0;
        }
        else if (command == "--cflatco") {
            const auto& ch = getCharacteristics();
            cout << fixed << setprecision(6) << ch.flattening << "\n";
            return 0;
        }
        else if (command == "--cvarcoii") {
            const auto& ch = getCharacteristics();
            cout << fixed << setprecision(6) << ch.varcoeff_II << "\n";
            return 0;
        }
        else if (command == "--centropy") {
            const auto& ch = getCharacteristics();
            cout << fixed << setprecision(6) << ch.entropy << "\n";
            return 0;
        }
        else if (command == "--sedgesharp") {
            if (!optimized && (variant < 1 || variant > 3)) {
                throw runtime_error("--sedgesharp requires -variant in {1,2,3} or -optimized");
            }
            if (optimized) {
                result = edge_sharpen_optimized(img);
                actionDescription = "Applied optimized edge sharpening";
            } else {
                switch (variant) {
                    case 1: result = edge_sharpen_type1(img); break;
                    case 2: result = edge_sharpen_type2(img); break;
                    case 3: result = edge_sharpen_type3(img); break;
                    default: throw runtime_error("Unsupported edge sharpening variant");
                }
                actionDescription = "Applied edge sharpening (variant " + to_string(variant) + ")";
            }
            produceImage = true;
            ensureOutput(slugify(command) + (optimized ? "_opt" : "_v" + to_string(variant)));
        }
        else if (command == "--orosenfeld") {
            switch (P) {
                case 1: case 2: case 4: case 8: case 16:
                    break;
                default:
                    throw runtime_error("--orosenfeld requires -P set to 1, 2, 4, 8, or 16");
            }
            result = rosenfeld_operator(img, P);
            produceImage = true;
            actionDescription = "Applied Rosenfeld operator (P=" + to_string(P) + ")";
            ensureOutput(slugify(command) + "_P" + to_string(P));
        }
        else {
            cerr << "Unknown command: " << command << "\n";
            return 1;
        }

        if (produceImage) {
            saveAsBMP(result, outputPath);
            cout << actionDescription << "\n";
            cout << "Saved: " << resolveAbsolutePath(outputPath) << "\n";
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
