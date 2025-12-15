#include "Morphology.h"
#include <iostream>

// --- StructElement Implementation ---

StructElement StructElement::createSquare3x3() {
    // 3x3 Block of 1s
    std::vector<int> data(9, 1);
    return StructElement(3, 3, data, 1, 1);
}

StructElement StructElement::createCross3x3() {
    // 0 1 0
    // 1 1 1
    // 0 1 0
    // Note: In standard erosion context, 0 usually implies "don't care" for the shape
    // but for specific structural definitions, we treat the shape explicitly.
    // For standard binary morphology, we usually only care about the 1s.
    std::vector<int> data = {
        0, 1, 0,
        1, 1, 1,
        0, 1, 0
    };
    return StructElement(3, 3, data, 1, 1);
}

std::vector<StructElement> StructElement::createGolayLSequence() {
    std::vector<StructElement> seq;
    
    // Golay L1 element (North) for Thinning
    // 0 0 0
    // x 1 x  (x = -1/don't care)
    // 1 1 1
    std::vector<int> base = {
         0, 0, 0,
        -1, 1, -1,
         1, 1, 1
    };
    
    // Generate 8 rotations
    for (int i = 0; i < 8; ++i) {
        seq.push_back(StructElement(3, 3, base, 1, 1));
        
        // Rotate 45 degrees clockwise for next element
        std::vector<int> next = base;
        // Mapping indices for 3x3 rotation
        // 0 1 2      3 0 1
        // 3 4 5  ->  6 4 2
        // 6 7 8      7 8 5
        next[0]=base[3]; next[1]=base[0]; next[2]=base[1];
        next[5]=base[2]; next[8]=base[5]; next[7]=base[8];
        next[6]=base[7]; next[3]=base[6]; next[4]=base[4]; // center stays
        base = next;
    }
    return seq;
}

// --- Basic Operations ---

CImg<unsigned char> morph_dilation(const CImg<unsigned char>& src, const StructElement& se) {
    CImg<unsigned char> out(src.width(), src.height(), 1, 1, 0);
    
    cimg_forXY(src, x, y) {
        if (src(x, y) == 0) continue; // Optimization
        
        // Imprint the SE onto the output
        for(int h = 0; h < se.height; ++h) {
            for(int w = 0; w < se.width; ++w) {
                if (se.data[h * se.width + w] == 1) {
                    int nx = x + (w - se.cx);
                    int ny = y + (h - se.cy);
                    if (nx >= 0 && nx < out.width() && ny >= 0 && ny < out.height()) {
                        out(nx, ny) = 255;
                    }
                }
            }
        }
    }
    return out;
}

CImg<unsigned char> morph_erosion(const CImg<unsigned char>& src, const StructElement& se) {
    CImg<unsigned char> out(src.width(), src.height(), 1, 1, 0);
    
    cimg_forXY(src, x, y) {
        bool fit = true;
        for(int h = 0; h < se.height; ++h) {
            for(int w = 0; w < se.width; ++w) {
                // If SE has a 1, Image MUST have a 1 (255)
                if (se.data[h * se.width + w] == 1) {
                    int nx = x + (w - se.cx);
                    int ny = y + (h - se.cy);
                    
                    // Check bounds and value
                    if (nx < 0 || nx >= src.width() || ny < 0 || ny >= src.height() || src(nx, ny) == 0) {
                        fit = false;
                        break;
                    }
                }
            }
            if (!fit) break;
        }
        if (fit) out(x, y) = 255;
    }
    return out;
}

CImg<unsigned char> morph_opening(const CImg<unsigned char>& src, const StructElement& se) {
    return morph_dilation(morph_erosion(src, se), se);
}

CImg<unsigned char> morph_closing(const CImg<unsigned char>& src, const StructElement& se) {
    return morph_erosion(morph_dilation(src, se), se);
}

CImg<unsigned char> morph_hmt(const CImg<unsigned char>& src, const StructElement& se) {
    CImg<unsigned char> out(src.width(), src.height(), 1, 1, 0);
    
    cimg_forXY(src, x, y) {
        bool match = true;
        for(int h = 0; h < se.height; ++h) {
            for(int w = 0; w < se.width; ++w) {
                int val = se.data[h * se.width + w];
                
                if (val == -1) continue; // Don't care
                
                int nx = x + (w - se.cx);
                int ny = y + (h - se.cy);
                
                int pixel = 0;
                if (nx >= 0 && nx < src.width() && ny >= 0 && ny < src.height()) {
                    pixel = (src(nx, ny) > 127) ? 1 : 0;
                }
                
                if (pixel != val) {
                    match = false;
                    break;
                }
            }
            if (!match) break;
        }
        if (match) out(x, y) = 255;
    }
    return out;
}

// M5 Implementation
CImg<unsigned char> morph_thinning(const CImg<unsigned char>& src) {
    CImg<unsigned char> curr = src;
    std::vector<StructElement> sequence = StructElement::createGolayLSequence();
    
    bool changed = true;
    int iteration = 0;
    
    std::cout << "[Morphology] Starting thinning (M5)..." << std::endl;
    
    while (changed) {
        changed = false;
        
        // Cycle through all 8 structural elements
        for (const auto& se : sequence) {
            // Find hits (pixels to remove)
            CImg<unsigned char> hits = morph_hmt(curr, se);
            
            // Remove hits: A = A \ hits
            cimg_forXY(curr, x, y) {
                if (hits(x, y) == 255 && curr(x, y) == 255) {
                    curr(x, y) = 0;
                    changed = true;
                }
            }
        }
        iteration++;
    }
    
    std::cout << "[Morphology] Thinning converged after " << iteration << " iterations." << std::endl;
    return curr;
}
