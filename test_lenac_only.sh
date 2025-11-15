#!/bin/bash

echo "================================================"
echo "  LENA COLOR IMAGE - COMPLETE FUNCTIONALITY TEST"
echo "================================================"
echo ""

mkdir -p output_images/lenac_demo

IMG="images/Color/lenac"
OUT="output_images/lenac_demo"

echo "Using: $IMG"
echo ""

echo "=== ELEMENTARY OPERATIONS ==="
echo "[1] Brightness +50..."
./build/imageProcessor --brightness -value=50 -input=$IMG -output=$OUT/01_bright50

echo "[2] Brightness -50..."
./build/imageProcessor --brightness -value=-50 -input=$IMG -output=$OUT/02_dark50

echo "[3] Contrast 1.8x..."
./build/imageProcessor --contrast -factor=1.8 -input=$IMG -output=$OUT/03_contrast18

echo "[4] Contrast 0.5x..."
./build/imageProcessor --contrast -factor=0.5 -input=$IMG -output=$OUT/04_contrast05

echo "[5] Negative..."
./build/imageProcessor --negative -input=$IMG -output=$OUT/05_negative

echo "[6] RGB shift..."
./build/imageProcessor --rgb -r=50 -g=-30 -b=20 -input=$IMG -output=$OUT/06_rgb_shift

echo ""
echo "=== GEOMETRIC OPERATIONS ==="
echo "[7] Horizontal flip..."
./build/imageProcessor --hflip -input=$IMG -output=$OUT/07_hflip

echo "[8] Vertical flip..."
./build/imageProcessor --vflip -input=$IMG -output=$OUT/08_vflip

echo "[9] Diagonal flip..."
./build/imageProcessor --dflip -input=$IMG -output=$OUT/09_dflip

echo "[10] Shrink 0.5x..."
./build/imageProcessor --shrink -factor=0.5 -input=$IMG -output=$OUT/10_shrink

echo "[11] Enlarge 2x..."
./build/imageProcessor --enlarge -factor=2.0 -input=$IMG -output=$OUT/11_enlarge

echo ""
echo "=== NOISE REMOVAL - IMPULSE ==="
IMG_IMPULSE="images/Color/Noise/lenac_impulse1"

echo "[12] Arithmetic mean k=3..."
./build/imageProcessor --amean -ksize=3 -input=$IMG_IMPULSE -output=$OUT/12_impulse_amean3

echo "[13] Arithmetic mean k=5..."
./build/imageProcessor --amean -ksize=5 -input=$IMG_IMPULSE -output=$OUT/13_impulse_amean5

echo "[14] Adaptive median (BEST for impulse)..."
./build/imageProcessor --adaptive -ksize=3 -smax=7 -input=$IMG_IMPULSE -output=$OUT/14_impulse_adaptive

echo ""
echo "=== NOISE REMOVAL - GAUSSIAN ==="
IMG_NORMAL="images/Color/Noise/lenac_normal1"

echo "[15] Arithmetic mean k=3 (BEST for Gaussian)..."
./build/imageProcessor --amean -ksize=3 -input=$IMG_NORMAL -output=$OUT/15_gaussian_amean3

echo "[16] Arithmetic mean k=5..."
./build/imageProcessor --amean -ksize=5 -input=$IMG_NORMAL -output=$OUT/16_gaussian_amean5

echo "[17] Adaptive median..."
./build/imageProcessor --adaptive -ksize=3 -smax=7 -input=$IMG_NORMAL -output=$OUT/17_gaussian_adaptive

echo ""
echo "=== QUALITY METRICS ==="
echo ""
echo "IMPULSE NOISE COMPARISON:"
echo "Original vs Noisy:"
./build/imageProcessor --psnr -ref=$IMG -cmp=$IMG_IMPULSE

echo ""
echo "Arithmetic Mean k=3:"
./build/imageProcessor --psnr -ref=$IMG -cmp=$OUT/12_impulse_amean3

echo "Adaptive Median (WINNER for impulse):"
./build/imageProcessor --psnr -ref=$IMG -cmp=$OUT/14_impulse_adaptive

echo ""
echo "GAUSSIAN NOISE COMPARISON:"
echo "Original vs Noisy:"
./build/imageProcessor --psnr -ref=$IMG -cmp=$IMG_NORMAL

echo ""
echo "Arithmetic Mean k=3 (WINNER for Gaussian):"
./build/imageProcessor --psnr -ref=$IMG -cmp=$OUT/15_gaussian_amean3

echo "Adaptive Median:"
./build/imageProcessor --psnr -ref=$IMG -cmp=$OUT/17_gaussian_adaptive

echo ""
echo "================================================"
echo "  ✓ ALL 17 TESTS COMPLETE!"
echo "================================================"
echo "Results in: $OUT/"
echo ""

# List all outputs
echo "Generated files:"
ls -1 $OUT/

echo ""
echo "Opening results..."
open $OUT/
