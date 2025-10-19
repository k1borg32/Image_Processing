#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <iostream>

#define cimg_display 0
#define cimg_use_bmp
#include "CImg.h"

using namespace std;
using namespace cimg_library;

struct Args {
    string command;
    unordered_map<string,string> kv;
};

Args parseArgs(int argc, char* argv[]);
bool readInt(const unordered_map<string,string>& kv, const string& key, int& out);
bool readFloat(const unordered_map<string,string>& kv, const string& key, float& out);
bool readString(const unordered_map<string,string>& kv, const string& key, string& out);

template <typename T>
inline T clampv(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }

inline bool isOdd(int x) { return x % 2 != 0; }

inline unsigned char getp(const CImg<unsigned char>& img, int x, int y, int c) {
    x = clampv(x, 0, img.width()  - 1);
    y = clampv(y, 0, img.height() - 1);
    c = clampv(c, 0, img.spectrum()- 1);
    return img(x, y, c);
}

string resolveAbsolutePath(const string& path);
void saveAsBMP(const CImg<unsigned char>& img, const string& outputPath);
void printHelp(const char* prog);

#endif 
