# Evaluation
___
This directory contains tests scripts for two indcators: __SSIM__ and __PSNR__

## SSIM
To modify the path, you need to modify the code directly

### run
```shell

python3 ssim.py

```

## PSNR
To modify the path, you need to modify the code directly

### run
```shell

python3 psnr.py

```

## Dir Contents:

1. **div3** is fixed-version but take the influence of channel size into account when calculating the weight;
2. **norm** is fixed-version but ignore the influence of channel size when calculating the weight;

3. **Fixed-point** & **Float-point** are comparison between Fixed and Float version code.

4. **SIMD_OP** is fixed-point SIMD version with dynamic loop;
5. **NORM_BETA** is scalar version developed by embedded business unit;

__psnr__ contains **".txt"** results of psnr;

__ssim__ contains **".txt"** results of ssim;

__in__ contains input images with noise;

__origin__ contains origin images;

## Sundries
run.py is a convenient scripts for batch operation;

eval_beta.py is a test script for two images' __SSIM__ and __PSNR__
```shell

python3 eval_beta.py "compare1" "compare2"  

```

gray.py is a script for gray-scale conversion.

main.c is a program for calculate ratio.