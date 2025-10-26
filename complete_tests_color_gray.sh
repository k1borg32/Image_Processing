#!/bin/bash
echo "=========================================="
echo "  COMPLETE ANALYSIS - Color + Grayscale"
echo "=========================================="

RESULTS="final_results"
mkdir -p $RESULTS/{color,gray}

# COLOR IMAGES
IMG_CLEAN_C="images/Color/lenac"
IMG_IMP_C="images/Color/Noise/lenac_impulse1"
IMG_GAUSS_C="images/Color/Noise/lenac_normal1"

# GRAYSCALE IMAGES - try different possible paths
if [ -f "images/Greyscale/lena.bmp" ]; then
    IMG_CLEAN_G="images/Greyscale/lena"
    IMG_IMP_G="images/Greyscale/Noise/lena_impulse1"
    IMG_GAUSS_G="images/Greyscale/Noise/lena_normal1"
elif [ -f "images/Gray/lena.bmp" ]; then
    IMG_CLEAN_G="images/Gray/lena"
    IMG_IMP_G="images/Gray/Noise/lena_impulse1"
    IMG_GAUSS_G="images/Gray/Noise/lena_normal1"
elif [ -f "images/Grayscale/lena.bmp" ]; then
    IMG_CLEAN_G="images/Grayscale/lena"
    IMG_IMP_G="images/Grayscale/Noise/lena_impulse1"
    IMG_GAUSS_G="images/Grayscale/Noise/lena_normal1"
else
    echo "⚠️  Grayscale images not found - checking available paths..."
    ls -la images/
    echo ""
    echo "Please check which folder contains grayscale images and update script paths"
    IMG_CLEAN_G=""
fi

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
echo "  PART 1: COLOR - IMPULSE NOISE"
echo "==============================================="

compute_all $IMG_CLEAN_C $IMG_IMP_C "Original vs Noisy (impulse)"

./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_IMP_C -output=$RESULTS/color/adaptive_impulse
compute_all $IMG_CLEAN_C $RESULTS/color/adaptive_impulse "Adaptive Median"

./build/imageProcessor --amean -ksize=3 -input=$IMG_IMP_C -output=$RESULTS/color/amean_impulse
compute_all $IMG_CLEAN_C $RESULTS/color/amean_impulse "Arithmetic Mean k=3"

echo ""
echo "==============================================="
echo "  PART 2: COLOR - GAUSSIAN NOISE"
echo "==============================================="

compute_all $IMG_CLEAN_C $IMG_GAUSS_C "Original vs Noisy (Gaussian)"

./build/imageProcessor --amean -ksize=3 -input=$IMG_GAUSS_C -output=$RESULTS/color/amean_gaussian
compute_all $IMG_CLEAN_C $RESULTS/color/amean_gaussian "Arithmetic Mean k=3"

./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_GAUSS_C -output=$RESULTS/color/adaptive_gaussian
compute_all $IMG_CLEAN_C $RESULTS/color/adaptive_gaussian "Adaptive Median"

# GRAYSCALE TESTS (only if paths found)
if [ -n "$IMG_CLEAN_G" ]; then
    echo ""
    echo "==============================================="
    echo "  PART 3: GRAYSCALE - IMPULSE NOISE"
    echo "==============================================="

    compute_all $IMG_CLEAN_G $IMG_IMP_G "Original vs Noisy (impulse)"

    ./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_IMP_G -output=$RESULTS/gray/adaptive_impulse
    compute_all $IMG_CLEAN_G $RESULTS/gray/adaptive_impulse "Adaptive Median"

    ./build/imageProcessor --amean -ksize=3 -input=$IMG_IMP_G -output=$RESULTS/gray/amean_impulse
    compute_all $IMG_CLEAN_G $RESULTS/gray/amean_impulse "Arithmetic Mean k=3"

    echo ""
    echo "==============================================="
    echo "  PART 4: GRAYSCALE - GAUSSIAN NOISE"
    echo "==============================================="

    compute_all $IMG_CLEAN_G $IMG_GAUSS_G "Original vs Noisy (Gaussian)"

    ./build/imageProcessor --amean -ksize=3 -input=$IMG_GAUSS_G -output=$RESULTS/gray/amean_gaussian
    compute_all $IMG_CLEAN_G $RESULTS/gray/amean_gaussian "Arithmetic Mean k=3"

    ./build/imageProcessor --adaptive -ksize=3 -smax=9 -input=$IMG_GAUSS_G -output=$RESULTS/gray/adaptive_gaussian
    compute_all $IMG_CLEAN_G $RESULTS/gray/adaptive_gaussian "Adaptive Median"
else
    echo ""
    echo "⚠️  SKIPPING GRAYSCALE TESTS - Images not found"
    echo "    Available folders:"
    ls -d images/*/ 2>/dev/null || echo "    (none found)"
fi

echo ""
echo "==============================================="
echo "  ✓ TESTS COMPLETE!"
echo "==============================================="
echo "Color images: $RESULTS/color/"
echo "Gray images:  $RESULTS/gray/"
echo ""
echo "Color images generated: $(ls $RESULTS/color/*.bmp 2>/dev/null | wc -l)"
echo "Gray images generated:  $(ls $RESULTS/gray/*.bmp 2>/dev/null | wc -l)"
echo ""
echo "Full log saved to: $RESULTS/complete_log.txt"

} | tee $RESULTS/complete_log.txt

echo ""
echo "=========================================="
echo "Next steps:"
echo "1. Check $RESULTS/complete_log.txt for all metrics"
echo "2. Copy images from $RESULTS/color/ and $RESULTS/gray/"
echo "3. Fill in report tables with the numbers"
echo "=========================================="

open $RESULTS/
