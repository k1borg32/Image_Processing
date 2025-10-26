#!/bin/bash

echo "=========================================="
echo "  PARAMETER STUDY - N9 VARIANT"
echo "=========================================="
echo ""

# Setup
RESULTS_DIR="parameter_study_results"
mkdir -p $RESULTS_DIR/images

IMG_CLEAN="images/Color/lenac"
IMG_IMPULSE="images/Color/Noise/lenac_impulse1"
IMG_GAUSSIAN="images/Color/Noise/lenac_normal1"

# Log everything
{
echo "Starting parameter study: $(date)"
echo ""

# Test 1: Adaptive median ksize
echo "=== TEST 1: Adaptive Median ksize ==="
for k in 3 5 7; do
    echo "Testing ksize=$k..."
    ./build/imageProcessor --adaptive -ksize=$k -smax=7 \
        -input=$IMG_IMPULSE -output=$RESULTS_DIR/images/adapt_k${k}
    echo -n "PSNR: "
    ./build/imageProcessor --psnr -ref=$IMG_CLEAN -cmp=$RESULTS_DIR/images/adapt_k${k}
    echo ""
done

# Test 2: Arithmetic mean kernel
echo "=== TEST 2: Arithmetic Mean kernel ==="
for k in 3 5 7; do
    echo "Testing k=$k..."
    ./build/imageProcessor --amean -ksize=$k \
        -input=$IMG_GAUSSIAN -output=$RESULTS_DIR/images/amean_k${k}
    echo -n "PSNR: "
    ./build/imageProcessor --psnr -ref=$IMG_CLEAN -cmp=$RESULTS_DIR/images/amean_k${k}
    echo ""
done

echo ""
echo "✓ COMPLETE! Results in: $RESULTS_DIR/"

} | tee $RESULTS_DIR/results.txt

echo ""
echo "Opening results folder..."
open $RESULTS_DIR/
