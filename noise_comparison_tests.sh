#!/bin/bash
echo "=========================================="
echo "  NOISE TYPE COMPARISON - SECTION 2"
echo "=========================================="

RESULTS="noise_comparison_results"
mkdir -p $RESULTS

IMG_CLEAN="images/Color/lenac"
IMG_IMPULSE="images/Color/Noise/lenac_impulse1"
IMG_GAUSSIAN="images/Color/Noise/lenac_normal1"

compute_all() {
    echo "--- $3 ---"
    echo -n "MSE:  "; ./build/imageProcessor --mse -ref=$1 -cmp=$2
    echo -n "PMSE: "; ./build/imageProcessor --pmse -ref=$1 -cmp=$2
    echo -n "SNR:  "; ./build/imageProcessor --snr -ref=$1 -cmp=$2
    echo -n "PSNR: "; ./build/imageProcessor --psnr -ref=$1 -cmp=$2
    echo -n "MD:   "; ./build/imageProcessor --md -ref=$1 -cmp=$2
    echo ""
}

{
echo ""
echo "=== IMPULSE NOISE ANALYSIS ==="
echo ""

compute_all $IMG_CLEAN $IMG_IMPULSE "1. Original vs Noisy Impulse"

./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_IMPULSE -output=$RESULTS/adaptive_impulse
compute_all $IMG_CLEAN $RESULTS/adaptive_impulse "2. Adaptive Median on Impulse"

./build/imageProcessor --amean -ksize=3 -input=$IMG_IMPULSE -output=$RESULTS/amean_impulse
compute_all $IMG_CLEAN $RESULTS/amean_impulse "3. Arithmetic Mean on Impulse"

echo ""
echo "=== GAUSSIAN NOISE ANALYSIS ==="
echo ""

compute_all $IMG_CLEAN $IMG_GAUSSIAN "1. Original vs Noisy Gaussian"

./build/imageProcessor --amean -ksize=3 -input=$IMG_GAUSSIAN -output=$RESULTS/amean_gaussian
compute_all $IMG_CLEAN $RESULTS/amean_gaussian "2. Arithmetic Mean on Gaussian"

./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_GAUSSIAN -output=$RESULTS/adaptive_gaussian
compute_all $IMG_CLEAN $RESULTS/adaptive_gaussian "3. Adaptive Median on Gaussian"

echo ""
echo "=========================================="
echo "✓ Noise comparison tests complete!"
echo "Images: $RESULTS/"
echo "=========================================="

} | tee $RESULTS/comparison_log.txt

open $RESULTS/
