#include "Segmentation.h"
#include <queue>
#include <cmath>
#include <vector>
#include <iostream>

struct Point {
    int x, y;
};

// Helper to calculate Euclidean color distance
// Works for 1 channel (Gray) or 3 channels (RGB)
double color_distance(const CImg<unsigned char>& img, int x1, int y1, int x2, int y2) {
    double sum = 0.0;
    // Iterate over all channels (Spectrum)
    // For Gray: c goes 0..0
    // For RGB: c goes 0..2
    cimg_forC(img, c) {
        double val1 = (double)img(x1, y1, c);
        double val2 = (double)img(x2, y2, c);
        double diff = val1 - val2;
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

CImg<unsigned char> region_growing(const CImg<unsigned char>& src, int seedX, int seedY, int threshold) {
    int w = src.width();
    int h = src.height();
    
    // Result mask: 255 = region, 0 = background
    CImg<unsigned char> mask(w, h, 1, 1, 0);
    CImg<bool> visited(w, h, 1, 1, false);
    
    // Basic bounds check
    if (seedX < 0 || seedX >= w || seedY < 0 || seedY >= h) {
        std::cerr << "[Segmentation] Error: Seed point (" << seedX << "," << seedY 
                  << ") is out of bounds (Size: " << w << "x" << h << ")." << std::endl;
        return mask;
    }
    
    std::queue<Point> q;
    q.push({seedX, seedY});
    visited(seedX, seedY) = true;
    mask(seedX, seedY) = 255;
    
    // 4-Connectivity neighbors
    const int dx[] = {0, 0, 1, -1};
    const int dy[] = {1, -1, 0, 0};
    
    int pixelCount = 0;
    
    while(!q.empty()) {
        Point p = q.front();
        q.pop();
        pixelCount++;
        
        for(int i = 0; i < 4; ++i) {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];
            
            if (nx >= 0 && nx < w && ny >= 0 && ny < h && !visited(nx, ny)) {
                // Calculate distance between the Seed (initial point) and the Neighbor.
                // Comparing to seed prevents "drift" (where the region slowly changes color).
                double dist = color_distance(src, seedX, seedY, nx, ny);
                
                if (dist <= threshold) {
                    visited(nx, ny) = true;
                    mask(nx, ny) = 255;
                    q.push({nx, ny});
                }
            }
        }
    }
    
    std::cout << "[Segmentation] Region Growing finished. Region size: " << pixelCount << " pixels." << std::endl;
    return mask;
}
