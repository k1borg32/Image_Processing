#include "NonLinearFilters.h"
#include <iostream>

using namespace std;

CImg<unsigned char> rosenfeld_operator(const CImg<unsigned char>& src, int P) {
    int w = src.width(), h = src.height(), s = src.spectrum();
    CImg<unsigned char> out(w, h, 1, s);
    
    for (int c = 0; c < s; ++c) {
        cimg_forXY(src, x, y) {
            int sum_positive = 0, sum_negative = 0;
            
            // Sum from n to n+P-1 (horizontal direction)
            for (int i = 0; i < P; ++i) {
                sum_positive += getp(src, x + i, y, c);
            }
            
            // Sum from n-P to n-1
            for (int i = 1; i <= P; ++i) {
                sum_negative += getp(src, x - i, y, c);
            }
            
            int diff = abs(sum_positive - sum_negative) / P;
            out(x, y, c) = (unsigned char)clampv(diff, 0, 255);
        }
    }
    return out;
}
