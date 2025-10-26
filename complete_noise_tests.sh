#!/bin/bash
echo "=========================================="
echo "  COMPLETE NOISE ANALYSIS (Color + Gray)"
echo "=========================================="

RESULTS="complete_noise_results"
mkdir -p $RESULTS/{color,gray}

# COLOR IMAGES
IMG_CLEAN_C="images/Color/lenac"
IMG_IMP_C="images/Color/Noise/lenac_impulse1"
IMG_GAUSS_C="images/Color/Noise/lenac_normal1"

# GRAYSCALE IMAGES
IMG_CLEAN_G="images/Greyscale/lena"
IMG_IMP_G="images/Greyscale/Noise/lena_impulse1"
IMG_GAUSS_G="images/Greyscale/Noise/lena_normal1"

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
echo "==============================================="
echo "  PART 1: COLOR IMAGES - IMPULSE NOISE"
echo "==============================================="

compute_all $IMG_CLEAN_C $IMG_IMP_C "Original vs Noisy (impulse)"

./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_IMP_C -output=$RESULTS/color/adaptive_impulse
compute_all $IMG_CLEAN_C $RESULTS/color/adaptive_impulse "Adaptive Median"

./build/imageProcessor --amean -ksize=3 -input=$IMG_IMP_C -output=$RESULTS/color/amean_impulse
compute_all $IMG_CLEAN_C $RESULTS/color/amean_impulse "Arithmetic Mean k=3"

./build/imageProcessor --amean -ksize=5 -input=$IMG_IMP_C -output=$RESULTS/color/amean5_impulse
compute_all $IMG_CLEAN_C $RESULTS/color/amean5_impulse "Arithmetic Mean k=5"

echo ""
echo "==============================================="
echo "  PART 2: COLOR IMAGES - GAUSSIAN NOISE"
echo "==============================================="

compute_all $IMG_CLEAN_C $IMG_GAUSS_C "Original vs Noisy (Gaussian)"

./build/imageProcessor --amean -ksize=3 -input=$IMG_GAUSS_C -output=$RESULTS/color/amean_gaussian
compute_all $IMG_CLEAN_C $RESULTS/color/amean_gaussian "Arithmetic Mean k=3"

./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_GAUSS_C -output=$RESULTS/color/adaptive_gaussian
compute_all $IMG_CLEAN_C $RESULTS/color/adaptive_gaussian "Adaptive Median"

echo ""
echo "==============================================="
echo "  PART 3: GRAYSCALE IMAGES - IMPULSE NOISE"
echo "==============================================="

compute_all $IMG_CLEAN_G $IMG_IMP_G "Original vs Noisy (impulse)"

./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_IMP_G -output=$RESULTS/gray/adaptive_impulse
compute_all $IMG_CLEAN_G $RESULTS/gray/adaptive_impulse "Adaptive Median"

./build/imageProcessor --amean -ksize=3 -input=$IMG_IMP_G -output=$RESULTS/gray/amean_impulse
compute_all $IMG_CLEAN_G $RESULTS/gray/amean_impulse "Arithmetic Mean k=3"

echo ""
echo "==============================================="
echo "  PART 4: GRAYSCALE IMAGES - GAUSSIAN NOISE"
echo "==============================================="

compute_all $IMG_CLEAN_G $IMG_GAUSS_G "Original vs Noisy (Gaussian)"

./build/imageProcessor --amean -ksize=3 -input=$IMG_GAUSS_G -output=$RESULTS/gray/amean_gaussian
compute_all $IMG_CLEAN_G $RESULTS/gray/amean_gaussian "Arithmetic Mean k=3"

./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_GAUSS_G -output=$RESULTS/gray/adaptive_gaussian
compute_all $IMG_CLEAN_G $RESULTS/gray/adaptive_gaussian "Adaptive Median"

echo ""
echo "==============================================="
echo "  ✓ ALL TESTS COMPLETE!"
echo "==============================================="
echo "Color images: $RESULTS/color/"
echo "Gray images: $RESULTS/gray/"
echo ""
ls -lh $RESULTS/color/ | tail -n +2 | wc -l | xargs echo "Color images generated:"
ls -lh $RESULTS/gray/ | tail -n +2 | wc -l | xargs echo "Gray images generated:"

} | tee $RESULTS/full_analysis_log.txt

open $RESULTS/
