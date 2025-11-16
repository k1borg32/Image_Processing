# Task 2 Image Processor (Windows Branch)

This branch contains the Windows-focused build of the Task 2 image-processing CLI. It implements every variant requested in the exercise (histogram equalisation H1–H5, characteristics C1–C6, linear filters S1–S6, non-linear filters O1–O6) and keeps the tooling Windows-friendly.

## Prerequisites
- Windows 10/11 with [MinGW-w64](https://www.mingw-w64.org/) installed
- `mingw32-make` and `g++` (MinGW) available on `PATH`
- Run commands from a **Developer PowerShell** or **Command Prompt**

## Building
- Preferred: run the Makefile directly
  ```powershell
  mingw32-make
  ```
  The executable is generated at `build\imageProcessor.exe`.
- Clean build artifacts:
  ```powershell
  mingw32-make clean
  ```
- Optional convenience: `build_task2_windows.bat` simply wraps the `mingw32-make` call. Use it only if you prefer double-click/one-liner builds; it is not required when you are already invoking `mingw32-make` directly.

## Running the CLI
Run commands through `cmd` (or prefix arguments with `--%` in PowerShell) so `-input=` style switches pass through:
```powershell
cmd /c "cd /d d:\image_processing\build && imageProcessor.exe --hpower -input=..\images\Gray\lena.bmp -output=..\output\lena_hpower.bmp"
```
### Histogram equalisation (H1–H5)
- `--huniform -gmin=0 -gmax=255`
- `--hexponent -alpha=1.0`
- `--hrayleigh [-alpha=45.0]`
- `--hpower -gmin=0 -gmax=255`
- `--hhyper -gmin=0 -gmax=255`
- `--histogram -channel=0` (saves histogram image)

### Image characteristics (C1–C6)
- `--characteristics -channel=0`
- Individual metrics: `--cmean`, `--cvariance`, `--cstdev`, `--cvarcoi`, `--casyco`, `--cflatco`, `--cvarcoii`, `--centropy`

### Linear filters (S1–S6)
- `--slowpass -mask=1|2|3`
- `--sedgesharp -variant=1|2|3` or `--sedgesharp -optimized`
- `--sexdeti` (detail extraction I)
- `--sexdetii` (detail extraction II)
- `--slaplace -mask=1|2|3`
- `--slineid`

### Non-linear filters (O1–O6)
- `--orobertsi`, `--orobertsii`
- `--osobel`
- `--okirsch` (`--okirsf` alias)
- `--orosenfeld -P=1|2|4|8|16`
- `--oll`

Notes:
- Outputs default to BMP files; directories are created automatically. When running from the `build` directory, prefix outputs with `..\` to land inside the repo (e.g., `-output=..\output\result`).
- `-alpha` controls the exponential and Rayleigh transforms (defaults: 1.0 and 45.0 respectively); `-mask` selects pre-defined convolution kernels; `-variant` chooses the S2 kernel.
- Channel indices are 0-based (`-channel=0` is the first channel).

## Notes
- All CImg operations run headless (`cimg_display 0`), with `cimg_use_bmp` enabled for BMP I/O.
- Output folders are created on demand; no manual `mkdir` is required.
- The macOS task branch remains untouched so teammates can continue development there independently.
