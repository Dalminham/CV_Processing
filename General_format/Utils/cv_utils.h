#ifndef __INCLUDE_CV_UTILS_H
#define __INCLUDE_CV_UTILS_H
#include "bmp.h"
#include "Mat.h"
float bmp_similar(uint8_t* src1, uint8_t* src2, uint32_t size);

//border copy function, symmetry fill the enlarged border; Max_radius:127
void copyMakeBorder(Mat_t *src, Mat_t *dst, int radius);
void bilateralFilter_simd(Mat_t* _src, Mat_t* _dst, int radius,
                     double sigmaColor, double sigmaSpace, uint8_t tag);
void bilateralFilter_simd_beta(Mat_t* _src, Mat_t* _dst, int radius,
                           double sigmaColor, double sigmaSpace);
        
void bilateralFilter_fraction(Mat_t* src, Mat_t* dst, int radius,
                        double sigma_color, double sigma_space);
#endif