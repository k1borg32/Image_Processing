#!/bin/bash
echo "=========================================="
echo "  PARAMETER ANALYSIS - N9 Variant"
echo "=========================================="

RESULTS="param_results"
mkdir -p $RESULTS/images

IMG_CLEAN="images/Color/lenac"
IMG_IMPULSE="images/Color/Noise/lenac_impulse1"
IMG_GAUSSIAN="images/Color/Noise/lenac_normal1"

compute_metrics() {
    echo "--- $3 ---"
    echo -n "MSE:  "; ./build/imageProcessor --mse -ref=$1 -cmp=$2
    echo -n "PMSE: "; ./build/imageProcessor --pmse -ref=$1 -cmp=$2
    echo -n "SNR:  "; ./build/imageProcessor --snr -ref=$1 -cmp=$2
    echo -n "PSNR: "; ./build/imageProcessor --psnr -ref=$1 -cmp=$2
    echo -n "MD:   "; ./build/imageProcessor --md -ref=$1 -cmp=$2
    echo ""
}

{
echo "TEST 1: Adaptive Median ksize Study (on Impulse Noise)"
echo ""

for k in 3 5 7; do
    OUT="$RESULTS/images/adaptive_k${k}_s9"
    ./build/imageProcessor --adaptive -ksize=$k -smax=9 -input=$IMG_IMPULSE -output=$OUT
    compute_metrics $IMG_CLEAN $OUT "Adaptive Median ksize=$k, smax=9"
done

echo ""
echo "TEST 2: Adaptive Median smax Study (on Impulse Noise)"
echo ""

for s in 5 7 9 11; do
    OUT="$RESULTS/images/adaptive_k3_s${s}"
    ./build/imageProcessor --adaptive -ksize=3 -smax=$s -input=$IMG_IMPULSE -output=$OUT
    compute_metrics $IMG_CLEAN $OUT "Adaptive Median ksize=3, smax=$s"
done

echo ""
echo "TEST 3: Arithmetic Mean Kernel Study (on Gaussian Noise)"
echo ""

for k in 3 5 7 9; do
    OUT="$RESULTS/images/amean_k${k}_gauss"
    ./build/imageProcessor --amean -ksize=$k -input=$IMG_GAUSSIAN -output=$OUT
    compute_metrics $IMG_CLEAN $OUT "Arithmetic Mean k=$k"
done

echo ""
echo "=========================================="
echo "✓ All parameter tests complete!"
echo "Results: $RESULTS/images/"
echo "Log: $RESULTS/full_log.txt"
echo "=========================================="

} | tee $RESULTS/full_log.txt

open $RESULTS/images/
