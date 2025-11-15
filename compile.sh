#!/bin/bash

echo "Compiling Task 2..."

# Compile with clang++ (macOS - no X11 needed)
clang++ -std=c++11 -O2 \
    src/main.cpp \
    src/Histogram.cpp \
    src/LinearFilters.cpp \
    src/NonLinearFilters.cpp \
    -I src \
    -o imageProcessor

echo "Compilation complete! Binary: ./imageProcessor"
