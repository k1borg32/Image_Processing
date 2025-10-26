#include "Utils.h"
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace cimg_library;

Args parseArgs(int argc, char* argv[]) {
    Args a;
    if (argc < 2) return a;
    a.command = argv[1];
    for (int i = 2; i < argc; ++i) {
        string token = argv[i];
        if (token.size() >= 2 && token[0] == '-' && token[1] != '-') {
            auto pos = token.find('=');
            if (pos == string::npos) {
                a.kv[token] = "1";
            } else {
                a.kv[token.substr(0, pos)] = token.substr(pos + 1);
            }
        }
    }
    return a;
}

bool readInt(const unordered_map<string,string>& kv, const string& key, int& out) {
    auto it = kv.find(key); if (it == kv.end()) return false;
    try { out = stoi(it->second); } catch (...) { return false; }
    return true;
}

bool readFloat(const unordered_map<string,string>& kv, const string& key, float& out) {
    auto it = kv.find(key); if (it == kv.end()) return false;
    try { out = stof(it->second); } catch (...) { return false; }
    return true;
}

bool readString(const unordered_map<string,string>& kv, const string& key, string& out) {
    auto it = kv.find(key); if (it == kv.end()) return false;
    out = it->second; return true;
}

static string ensureBmpExtension(const string& path) {
    if (path.length() >= 4) {
        string ext = path.substr(path.length() - 4);
        for (char& c : ext) c = tolower(c);
        if (ext == ".bmp") {
            return path; 
        }
    }
    return path + ".bmp";
}

string resolveAbsolutePath(const string& path) {
    string pathWithExt = ensureBmpExtension(path);
    
    // Check if path is already absolute (starts with /)
    if (pathWithExt.length() >= 1 && pathWithExt[0] == '/') {
        return pathWithExt;
    }
    
    // Get current working directory
    char currentDir[PATH_MAX];
    if (getcwd(currentDir, sizeof(currentDir)) == nullptr) {
        return pathWithExt; // Fallback to relative path
    }
    
    string absPath = string(currentDir) + "/" + pathWithExt;
    
    return absPath;
}

void saveAsBMP(const CImg<unsigned char>& img, const string& outputPath) {
    string absPath = resolveAbsolutePath(outputPath);
    
    // Create directory if it doesn't exist (POSIX version)
    size_t pos = absPath.find_last_of("/");
    if (pos != string::npos) {
        string dir = absPath.substr(0, pos);
        
        // Create directory with permissions 0755 (rwxr-xr-x)
        // This will fail silently if directory already exists
        mkdir(dir.c_str(), 0755);
    }
    
    img.save_bmp(absPath.c_str());
}

void printHelp(const char* prog) {
    cout << "\nImage Processing CLI (CImg I/O only)\n";
    cout << "Usage:\n";
    cout << "  " << prog << " --command -arg=value [-arg=value ...]\n\n";

    cout << "General args:\n";
    cout << "  -input=path     Input image path \n";
    cout << "  -output=path    Output image path \n";
    cout << "  -value=int      Integer value (e.g., brightness)\n";
    cout << "  -factor=float   Scaling/contrast factor [use quotes for decimals, e.g., \"0.4\"]\n";
    cout << "  -ksize=int      Odd kernel size >=3 (filters)\n";
    cout << "  -smax=int       Odd max window size >= ksize (adaptive median)\n";
    cout << "  -ref=path       Reference image \n";
    cout << "  -cmp=path       Compared image \n\n";

    cout << "Commands:\n";
    cout << "  --help                       Show this help\n";
    cout << "  --brightness  -value=INT     Brightness [-255..255]\n";
    cout << "  --contrast    -factor=FLOAT  Linear contrast [0.1..3.0] (in quotes)\n";
    cout << "  --negative                    Invert colors\n";
    cout << "  --rgb        -r=INT -g=INT -b=INT  Add per-channel offsets\n";
    cout << "  --hflip                       Horizontal flip\n";
    cout << "  --vflip                       Vertical flip\n";
    cout << "  --dflip                       Diagonal flip (transpose)\n";
    cout << "  --shrink     -factor=FLOAT   Scale with factor < 1.0 (in quotes)\n";
    cout << "  --enlarge    -factor=FLOAT   Scale with factor > 1.0 (in quotes)\n";
    cout << "  --amean      -ksize=ODD      Arithmetic mean filter (box)\n";
    cout << "  --adaptive   -ksize=ODD -smax=ODD  Adaptive median filter\n";
    cout << "  --mse        -ref=X -cmp=Y   Mean square error\n";
    cout << "  --pmse       -ref=X -cmp=Y   Peak mean square error\n";
    cout << "  --snr        -ref=X -cmp=Y   Signal-to-noise ratio [dB]\n";
    cout << "  --psnr       -ref=X -cmp=Y   Peak signal-to-noise ratio [dB]\n";
    cout << "  --md         -ref=X -cmp=Y   Maximum difference\n\n";

    cout << "Examples (extension optional, use quotes for decimals):\n";
    cout << "  " << prog << " --brightness -value=40 -input=images/Color/lenac -output=output_images/lenac_bright\n";
    cout << "  " << prog << " --contrast -factor=1.8 -input=images/Binary/girlbw -output=output_images/girl_contrast\n";
    cout << "  " << prog << " --shrink -factor=0.5 -input=images/Binary/mandrilbw -output=output_images/mandril_shrink\n";
    cout << "  " << prog << " --psnr -ref=images/Gray/lena -cmp=output_images/lena_filtered\n\n";

    cout << "To run all of the commands in one go, run automated script\n";
    cout << "    ./test_comprehensive.sh\n";
    cout << "This script will exercise all filters, enhancements, and metrics.\n";
}
