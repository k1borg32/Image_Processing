@echo off
echo ================================================
echo   COMPREHENSIVE IMAGE PROCESSING TEST SUITE
echo ================================================
echo.
if not exist output_images mkdir output_images

echo === PART 1: ELEMENTARY OPERATIONS ===
echo [1] Brightness grayscale...
.\build\imageProcessor.exe --brightness -value=50 -input=images\Gray\lena -output=output_images\01_lena_bright50

echo [2] Brightness color (dark)...
.\build\imageProcessor.exe --brightness -value=-30 -input=images\Color\lenac -output=output_images\02_lenac_dark30

echo [3] Contrast high...
.\build\imageProcessor.exe --contrast -factor="1.8" -input=images\Gray\mandril -output=output_images\04_mandril_contrast_high

echo [4] Negative grayscale...
.\build\imageProcessor.exe --negative -input=images\Gray\bridge -output=output_images\06_bridge_negative

echo [5] RGB shift color...
.\build\imageProcessor.exe --rgb -r=40 -g=-20 -b=15 -input=images\Color\lenac -output=output_images\08_lenac_rgb_shift

echo.
echo === PART 2: GEOMETRIC OPERATIONS ===
echo [6] Horizontal flip...
.\build\imageProcessor.exe --hflip -input=images\Gray\bird -output=output_images\09_bird_hflip

echo [7] Vertical flip...
.\build\imageProcessor.exe --vflip -input=images\Gray\boat -output=output_images\10_boat_vflip

echo [8] Shrink to half...
.\build\imageProcessor.exe --shrink -factor="0.5" -input=images\Gray\pentagon -output=output_images\12_pentagon_shrink_half

echo [9] Enlarge 2x...
.\build\imageProcessor.exe --enlarge -factor="2.0" -input=images\Binary\mandrilbw -output=output_images\13_mandrilbw_enlarge_2x

echo.
echo === PART 3: NOISE REMOVAL (GRAY IMAGES) ===
echo [10] Impulse noise - Adaptive median...
.\build\imageProcessor.exe --adaptive -ksize=3 -smax=7 -input=images\Gray\Noise\lena_impulse1 -output=output_images\14_lena_gray_impulse1_adaptive

echo [11] Impulse noise - Arithmetic mean...
.\build\imageProcessor.exe --amean -ksize=3 -input=images\Gray\Noise\lena_impulse2 -output=output_images\15_lena_gray_impulse2_amean

echo [12] Normal noise - Arithmetic mean 3x3...
.\build\imageProcessor.exe --amean -ksize=3 -input=images\Gray\Noise\lena_normal1 -output=output_images\17_lena_gray_normal1_amean3

echo [13] Normal noise - Arithmetic mean 5x5...
.\build\imageProcessor.exe --amean -ksize=5 -input=images\Gray\Noise\lena_normal2 -output=output_images\18_lena_gray_normal2_amean5

echo [14] Uniform noise - Adaptive median...
.\build\imageProcessor.exe --adaptive -ksize=3 -smax=7 -input=images\Gray\Noise\lena_uniform1 -output=output_images\20_lena_gray_uniform1_adaptive

echo [15] Uniform noise - Arithmetic mean...
.\build\imageProcessor.exe --amean -ksize=5 -input=images\Gray\Noise\lena_uniform2 -output=output_images\21_lena_gray_uniform2_amean

echo.
echo === PART 4: NOISE REMOVAL (COLOR IMAGES) ===
echo [16] Color impulse - Adaptive median...
.\build\imageProcessor.exe --adaptive -ksize=3 -smax=7 -input=images\Color\Noise\lenac_impulse1 -output=output_images\23_lenac_impulse1_adaptive

echo [17] Color normal - Arithmetic mean...
.\build\imageProcessor.exe --amean -ksize=3 -input=images\Color\Noise\lenac_normal1 -output=output_images\26_lenac_normal1_amean3

echo [18] Color uniform - Adaptive median...
.\build\imageProcessor.exe --adaptive -ksize=3 -smax=7 -input=images\Color\Noise\lenac_uniform1 -output=output_images\29_lenac_uniform1_adaptive

echo.
echo === PART 5: SIMILARITY METRICS ===
echo [19] PSNR: Original vs Impulse Noisy...
.\build\imageProcessor.exe --psnr -ref=images\Gray\lena -cmp=images\Gray\Noise\lena_impulse1

echo [20] PSNR: Original vs Filtered Impulse...
.\build\imageProcessor.exe --psnr -ref=images\Gray\lena -cmp=output_images\14_lena_gray_impulse1_adaptive

echo [21] SNR: Original vs Normal Filtered...
.\build\imageProcessor.exe --snr -ref=images\Gray\lena -cmp=output_images\17_lena_gray_normal1_amean3

echo [22] MSE: Original vs Uniform Noisy...
.\build\imageProcessor.exe --mse -ref=images\Gray\lena -cmp=images\Gray\Noise\lena_uniform1

echo [23] MD: Original vs Filtered Uniform...
.\build\imageProcessor.exe --md -ref=images\Gray\lena -cmp=output_images\20_lena_gray_uniform1_adaptive

echo.
echo ================================================
echo   ALL TESTS COMPLETE!
echo ================================================
echo Results saved in: output_images\
echo.
start output_images
pause
