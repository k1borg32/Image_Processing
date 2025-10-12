#include <iostream>  // For input/output operations (cout, cin, endl)
#include <string>    // For string class and string operations
#include <exception> // For exception handling (try/catch blocks)

// CImg library configuration - these must be defined BEFORE including CImg.h
#define cimg_display 0   // Disable CImg's display functionality (we only need file I/O)
#define cimg_use_bmp     // Enable BMP file format support
#include "CImg.h"        // Include the CImg library header

// Use standard namespace to avoid writing std:: everywhere
using namespace std;
using namespace cimg_library;  // CImg's namespace

//#####################
// FUNCTION DEFINITIONS
//#####################

/**
 * BRIGHTNESS FUNCTION
 * Purpose: Adjust image brightness by adding/subtracting a constant value to each pixel
 * Algorithm: newPixel = originalPixel + brightnessValue
 * Parameters: 
 *   - param: brightness adjustment value (string, will be converted to int)
 *   - inputFile: path to input BMP file
 *   - outputFile: path where processed image will be saved
 */
void doBrightness(string param, string inputFile, string outputFile) {
    cout << "Function doBrightness invoked with param: " << param << endl;
    
    try {
        // STEP 1: Load the image from file
        // CImg<unsigned char> creates an image object where each pixel value is 0-255
        CImg<unsigned char> image(inputFile.c_str());
        cout << "Loaded image: " << inputFile << " (" << image.width() << "x" << image.height() << ")" << endl;
        
        // STEP 2: Convert string parameter to integer
        int brightness = stoi(param);  // stoi = "string to integer"
        cout << "Adjusting brightness by: " << brightness << endl;
        
        // STEP 3: Process every pixel in the image
        for (int x = 0; x < image.width(); x++) {      // Loop through width (columns)
            for (int y = 0; y < image.height(); y++) {  // Loop through height (rows)
                for (int c = 0; c < image.spectrum(); c++) { // Loop through color channels (R,G,B)
                    // Get current pixel value at position (x,y) in channel c
                    int pixel = image(x, y, c);
                    
                    // Apply brightness adjustment
                    pixel += brightness;
                    
                    // CLAMP values to valid range (0-255)
                    // This prevents overflow/underflow
                    if (pixel < 0) pixel = 0;       // Too dark → make it black (0)
                    if (pixel > 255) pixel = 255;   // Too bright → make it white (255)
                    
                    // Set the new pixel value back to the image
                    image(x, y, c) = pixel;
                }
            }
        }
        
        // STEP 4: Save the processed image to output file
        image.save_bmp(outputFile.c_str());  // .c_str() converts string to C-style string
        cout << "Saved processed image to: " << outputFile << endl;
        
    } catch (const exception& e) {
        // Handle any errors that occur during processing
        cout << "Error: " << e.what() << endl;
    }
}

/**
 * CONTRAST FUNCTION
 * Purpose: Adjust image contrast by scaling pixel values around middle gray (128)
 * Algorithm: newPixel = (originalPixel - 128) * contrastFactor + 128
 * Why 128? It's middle gray (halfway between 0 and 255)
 * Parameters:
 *   - param: contrast factor (string, will be converted to float)
 *   - inputFile: path to input BMP file  
 *   - outputFile: path where processed image will be saved
 */
void doContrast(string param, string inputFile, string outputFile) {
    cout << "Function doContrast invoked with param: " << param << endl;
    
    try {
        // STEP 1: Load image
        CImg<unsigned char> image(inputFile.c_str());
        cout << "Loaded image: " << inputFile << " (" << image.width() << "x" << image.height() << ")" << endl;
        
        // STEP 2: Convert string parameter to floating-point number
        float contrast = stof(param);  // stof = "string to float"
        cout << "Adjusting contrast by factor: " << contrast << endl;
        
        // STEP 3: Process every pixel
        for (int x = 0; x < image.width(); x++) {
            for (int y = 0; y < image.height(); y++) {
                for (int c = 0; c < image.spectrum(); c++) {
                    int pixel = image(x, y, c);
                    
                    // CONTRAST ALGORITHM EXPLANATION:
                    // 1. Subtract 128 to center around 0 (-128 to +127 range)
                    // 2. Multiply by contrast factor (>1 = more contrast, <1 = less contrast)  
                    // 3. Add 128 back to return to 0-255 range
                    int newPixel = (int)((pixel - 128.0f) * contrast + 128.0f);
                    
                    // CLAMP to valid range
                    if (newPixel < 0) newPixel = 0;
                    if (newPixel > 255) newPixel = 255;
                    
                    image(x, y, c) = newPixel;
                }
            }
        }
        
        // STEP 4: Save result
        image.save_bmp(outputFile.c_str());
        cout << "Saved processed image to: " << outputFile << endl;
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

/**
 * GRAYSCALE FUNCTION  
 * Purpose: Convert color image to grayscale using weighted average
 * Algorithm: gray = 0.299*Red + 0.587*Green + 0.114*Blue
 * Why these weights? Human eye is most sensitive to green, least to blue
 * Parameters:
 *   - inputFile: path to input BMP file
 *   - outputFile: path where processed image will be saved  
 */
void doGrayscale(string inputFile, string outputFile) {
    cout << "Function doGrayscale invoked" << endl;
    
    try {
        // STEP 1: Load image
        CImg<unsigned char> image(inputFile.c_str());
        cout << "Loaded image: " << inputFile << " (" << image.width() << "x" << image.height() << ")" << endl;
        
        // STEP 2: Convert to grayscale
        for (int x = 0; x < image.width(); x++) {
            for (int y = 0; y < image.height(); y++) {
                // Check if image has at least 3 color channels (RGB)
                if (image.spectrum() >= 3) {
                    // Get RGB values at current pixel position
                    float r = image(x, y, 0);  // Red channel (channel 0)
                    float g = image(x, y, 1);  // Green channel (channel 1) 
                    float b = image(x, y, 2);  // Blue channel (channel 2)
                    
                    // GRAYSCALE ALGORITHM: Weighted average
                    // These weights reflect human eye sensitivity to different colors
                    int gray = (int)(0.299f * r + 0.587f * g + 0.114f * b);
                    
                    // Set all channels to the same gray value
                    // This creates a grayscale image where R=G=B for each pixel
                    image(x, y, 0) = gray;  // Set red channel
                    image(x, y, 1) = gray;  // Set green channel  
                    image(x, y, 2) = gray;  // Set blue channel
                }
            }
        }
        
        // STEP 3: Save result
        image.save_bmp(outputFile.c_str());
        cout << "Saved processed image to: " << outputFile << endl;
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

//##########################
// MAIN FUNCTION - PROGRAM ENTRY POINT
//##########################

/**
 * MAIN FUNCTION
 * Purpose: Parse command-line arguments and call appropriate processing function
 * Parameters:
 *   - argc: argument count (number of command-line parameters)
 *   - argv: argument vector (array of command-line parameters as strings)
 * 
 * Command-line format: program.exe <command> [parameter] <input.bmp> [output.bmp]
 */
int main(int argc, char* argv[]) {
    cout << "\n\nImage Processing Application" << endl;
    cout << "Uses CImg library for BMP file I/O\n" << endl;

    // CASE 1: No arguments provided
    if (argc == 1) {
        cout << "No command line parameters given.\n" << endl;
        cout << "Usage: " << argv[0] << " <command> [parameter] <input.bmp> [output.bmp]" << endl;
        cout << "Commands:" << endl;
        cout << "  --brightness <value>   Adjust brightness (-255 to 255)" << endl;
        cout << "  --contrast <factor>    Adjust contrast (0.1 to 3.0)" << endl;
        cout << "  --grayscale           Convert to grayscale" << endl;
        return 0;  // Exit program
    }

    // CASE 2: Too few arguments
    if (argc < 3) {
        cout << "Too few command line parameters given.\n" << endl;
        return 0;
    }

    // ARGUMENT PARSING
    // argv[0] = program name (automatic)
    // argv[1] = command (--brightness, --contrast, --grayscale)
    // argv[2] = parameter OR input file (depends on command)
    string command = argv[1];  // First argument is always the command
    
    // COMMAND PROCESSING - Check which operation to perform
    
    if (command == "--brightness") {
        // BRIGHTNESS COMMAND: needs parameter + input file + optional output file
        if (argc < 4) {
            cout << "Error: --brightness requires a parameter and input file" << endl;
            return 0;
        }
        string param = argv[2];      // Brightness adjustment value
        string inputFile = argv[3];  // Input BMP file path
        string outputFile = (argc > 4) ? argv[4] : "output.bmp";  // Output file (default if not specified)
        doBrightness(param, inputFile, outputFile);
        
    } else if (command == "--contrast") {
        // CONTRAST COMMAND: needs parameter + input file + optional output file  
        if (argc < 4) {
            cout << "Error: --contrast requires a parameter and input file" << endl;
            return 0;
        }
        string param = argv[2];      // Contrast factor
        string inputFile = argv[3];  // Input BMP file path
        string outputFile = (argc > 4) ? argv[4] : "output.bmp";  // Output file
        doContrast(param, inputFile, outputFile);
        
    } else if (command == "--grayscale") {
        // GRAYSCALE COMMAND: needs only input file + optional output file (no parameter)
        string inputFile = argv[2];  // Input BMP file path
        string outputFile = (argc > 3) ? argv[3] : "output.bmp";  // Output file
        doGrayscale(inputFile, outputFile);
        
    } else {
        // UNKNOWN COMMAND
        cout << "Unknown command: " + command << endl;
        cout << "Available commands: --brightness, --contrast, --grayscale" << endl;
    }

    cout << endl;
    return 0;  // Program completed successfully
}