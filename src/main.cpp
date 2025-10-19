#include "ImageProcessor.h"
#include "Metrics.h"
#include "Utils.h"

using namespace std;
using namespace cimg_library;

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 2) { printHelp(argv[0]); return 0; }
    Args a = parseArgs(argc, argv);
    if (a.command == "--help" || a.command.empty()) { printHelp(argv[0]); return 0; }

    try {
        if (a.command == "--mse" || a.command == "--pmse" || a.command == "--snr" || 
            a.command == "--psnr" || a.command == "--md") {
            string ref, cmp;
            if (!readString(a.kv, "-ref", ref) || !readString(a.kv, "-cmp", cmp))
                throw runtime_error("Metrics require -ref and -cmp");
            auto P = load_pair(ref, cmp);
            if      (a.command == "--mse")  cout << "MSE = "  << metric_MSE(P)     << "\n";
            else if (a.command == "--pmse") cout << "PMSE = " << metric_PMSE(P)    << "\n";
            else if (a.command == "--snr")  cout << "SNR (dB) = "  << metric_SNR_dB(P)  << "\n";
            else if (a.command == "--psnr") cout << "PSNR (dB) = " << metric_PSNR_dB(P) << "\n";
            else if (a.command == "--md")   cout << "MD = "   << metric_MD(P)      << "\n";
            return 0;
        }

        string inputFile, outputFile;
        if (!readString(a.kv, "-input", inputFile) || !readString(a.kv, "-output", outputFile))
            throw runtime_error("Most commands require -input and -output");

        ImageProcessor processor;
        if (!processor.loadImage(inputFile)) {
            throw runtime_error("Failed to load input image");
        }

        if (a.command == "--brightness") {
            int value = 0;
            if (!readInt(a.kv, "-value", value)) throw runtime_error("--brightness requires -value");
            processor.applyBrightness(value);
        }
        else if (a.command == "--contrast") {
            float factor = 1.0f;
            if (!readFloat(a.kv, "-factor", factor)) throw runtime_error("--contrast requires -factor");
            processor.applyContrast(factor);
        }
        else if (a.command == "--negative") {
            processor.applyNegative();
        }
        else if (a.command == "--rgb") {
            int r=0, g=0, b=0;
            readInt(a.kv, "-r", r); readInt(a.kv, "-g", g); readInt(a.kv, "-b", b);
            processor.applyRGBOffset(r, g, b);
        }
        else if (a.command == "--hflip") {
            processor.applyHorizontalFlip();
        }
        else if (a.command == "--vflip") {
            processor.applyVerticalFlip();
        }
        else if (a.command == "--dflip") {
            processor.applyDiagonalFlip();
        }
        else if (a.command == "--shrink") {
            float factor = 0.5f;
            readFloat(a.kv, "-factor", factor);
            processor.applyShrink(factor);
        }
        else if (a.command == "--enlarge") {
            float factor = 2.0f;
            readFloat(a.kv, "-factor", factor);
            processor.applyEnlarge(factor);
        }
        else if (a.command == "--amean") {
            int ksize = 3;
            readInt(a.kv, "-ksize", ksize);
            processor.applyArithmeticMean(ksize);
        }
        else if (a.command == "--adaptive") {
            int ksize = 3, smax = 7;
            readInt(a.kv, "-ksize", ksize);
            readInt(a.kv, "-smax", smax);
            processor.applyAdaptiveMedian(ksize, smax);
        }
        else {
            throw runtime_error("Unknown command: " + a.command + " (try --help)");
        }

        if (!processor.saveImage(outputFile)) {
            throw runtime_error("Failed to save output image");
        }

        cout << "✓ Processing complete!" << endl;

    } catch (const CImgIOException& e) {
        cerr << "CImg I/O error: " << e.what() << "\n";
        return 1;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
