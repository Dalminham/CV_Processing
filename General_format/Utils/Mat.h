#ifndef __INCLUDE_MAT_H
#define __INCLUDE_MAT_H

#include <stdint.h>
#include "bmp.h"

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}__attribute__ ((packed)) rgb_t;

typedef union
{
    rgb_t * rgb;
    uint8_t * gray;
}pixel_t;

typedef struct mat
{
    pixel_t data;        //image datq
    uint32_t width;     //image width
    uint32_t height;    //image height
    uint8_t cn;         //channel size
}Mat_t;

void bmp2Mat(BMP *bmp, Mat_t* mat);
void Mat2bmp(Mat_t * mat, uint8_t *bmp_data);
void Mat2NewBmp(Mat_t * mat, BMP* src, BMP*dst);
void Mat_free(Mat_t * mat);
void copy_MatHead(Mat_t *src, Mat_t* dst);

//For evaluation
void Mat2Plane(Mat_t *mat, image_t *image);
void Plane2Mat(image_t *image, Mat_t *mat);

#endif