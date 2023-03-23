#include "bilateralFilter_simd.h"
#include <math.h>
#include"../Utils/cv_utils.h"

#define BF

#ifdef BF
static void bilateralFilter_float(Mat_t* src, Mat_t* dst, int radius, double sigma_color, double sigma_space)
{
    int cn =  src->cn;
    int i, j, diameter, maxk, size;

    if(sigma_color <= 0)
        sigma_color = 1;
    if(sigma_space <= 0)
        sigma_space = 1;

    double gauss_color_coeff = -0.5/(sigma_color * sigma_color)/(cn*cn);
    double gauss_space_coeff = -0.5/(sigma_space * sigma_space);

    if(radius <= 0)
        radius = (int)(sigma_space*1.5);

    radius = radius>1 ? radius: 1;

    diameter = radius*2 + 1;


    Mat_t tmp;
    copyMakeBorder( src, &tmp, radius);

    size = dst->width * dst->height * cn;
    if(cn == 1){
        dst->data.gray = (uint8_t*)malloc(size*sizeof(uint8_t));
        memset(dst->data.gray, 0, size);
    }else{
        dst->data.rgb = (rgb_t*)malloc(size*sizeof(uint8_t));
        memset(dst->data.rgb, 0, size);
    }

    float * color_weight = (float*)malloc(256*cn*sizeof(float));
    float * space_weight = (float*)malloc(diameter * diameter * sizeof(float));
    int * space_ofs = (int*)malloc(diameter * diameter * sizeof(int));

    //printf("Color_Weights, limit %d:\n",256*cn);
    //inintialize color weight table
    for(i=0; i<256 * cn; i++){
        color_weight[i] = (float)exp(i*i*gauss_color_coeff);
        //printf("cw[%d]=%f\n",i, color_weight[i]);
    }

    //printf("\nSpace_Weights, limit %d:\n", diameter *diameter);
    for(i=-radius, maxk = 0; i <= radius; i++)
    {
        j = -radius;

        for(; j <= radius; j++)
        {
            double r = sqrt((double)i*i + (double) j*j);

            if (r > radius)
                continue;

            space_weight[maxk] = (float)exp(r*r*gauss_space_coeff);
            //printf("sw[%d]=%f\n",maxk,space_weight[maxk]);
            space_ofs[maxk++] = (int)(i * tmp.width*cn + j*cn);
        }
    }
    //printf("Space_max_k=%d\n",maxk);
    //exit(0);

    pr_debug("[%s]bilateralInvoker start\n",__func__);
    bilateralFilterInvoker_float(dst, &tmp, radius, maxk, space_ofs, space_weight, color_weight);
    pr_debug("[%s]bilateralInvoker end\n",__func__);
    free(color_weight);
    free(space_weight);
    free(space_ofs);
}

static void bilateralFilter_fixed(Mat_t*src, Mat_t* dst, int radius, double sigma_color, double sigma_space)
{
    //channel size. At present, there are only two cases:
    //1 and 3
    int cn = src->cn;
    //variable declaration
    /*index val; i, j;
    //diameter...;
    //filter size: maxk
    //img size: size*/
    int i, j, diameter, maxk, size;

    //Effective guarantee of variable
    if(sigma_color <= 0)
        sigma_color = 1;
    if(sigma_space <= 0)
        sigma_space = 1;

    //weight denominator pre calculation
    double gauss_color_coeff = -0.5/(sigma_color * sigma_color)/(cn * cn);
    double gauss_space_coeff = -0.5/(sigma_space * sigma_space);

    //Assign more bit width to more sensitive weight
    //uint32_t smax = (uint32_t)(pow(2,(10))-1);
    //uint32_t cmax = (uint32_t)(pow(2,(14)));
    //uint32_t max_chk = smax * cmax * 256;
    //printf("maximu=%ud\n",max_chk);

    //Effective guarantee of variable
    if(radius <= 0)
        radius = (int)(sigma_space*1.5);

    radius = radius>1 ? radius: 1;

    diameter = radius*2 + 1;

    //Expand the boundary to eliminate the branching judgement
    //of the boundary in the subsequent calculation
    Mat_t tmp;
    copyMakeBorder( src, &tmp, radius);

    size = dst->width * dst->height * cn;
    if(cn == 1){
        dst->data.gray = (uint8_t*)malloc(size*sizeof(uint8_t));
        memset(dst->data.gray, 0, size);
    }else{
        dst->data.rgb = (rgb_t*)malloc(size*sizeof(uint8_t));
        memset(dst->data.rgb, 0, size);
    }

    //initialize Look-Up-Table
    //The weight table provides the weight of the filtering formular;
    //The offest table provides offsets of other pixels in the window with
    //the center point as the base address.
    uint32_t * color_weight = (uint32_t*)malloc(256*cn*sizeof(uint32_t));
    uint32_t * space_weight = (uint32_t*)malloc(diameter * diameter * sizeof(uint32_t));
    int * space_ofs = (int*)malloc(diameter * diameter * sizeof(int));
    //printf("Color_weight:\n");
    for(i=0; i<256 *cn; i++){
        //The maximum of pixel val is 255;
        //Thus, the maximum of weight = sqrt(2^32/256)
        color_weight[i] = (uint32_t)(16384 * exp(i*i*gauss_color_coeff));
        //printf("cw[%d]=%d\n",i, color_weight[i]);
    }

    //printf("Space_weight:\n");
    for(i=-radius, maxk = 0; i <= radius; i++)
    {
        j = -radius;

        for(; j <= radius; j++)
        {
            double r = sqrt((double)i*i + (double) j*j);

            if(r > radius)
                continue;

            space_weight[maxk] = (uint32_t)(16384 * exp(r*r*gauss_space_coeff));
            //printf("sw[%d]=%d\n",maxk, space_weight[maxk]);
            space_ofs[maxk++] = (int)(i * tmp.width*cn + j*cn);
        }
    }

    bilateralFilterInvoker_fixed(dst, &tmp, radius, maxk, space_ofs, space_weight, color_weight);

    free(color_weight);
    free(space_weight);
    free(space_ofs);
}

static void bilateralFilter_fixed_beta(Mat_t*tmp, Mat_t* dst, int radius, double sigma_color, double sigma_space)
{
    //channel size. At present, there are only two cases:
    //1 and 3
    int cn = tmp->cn;
    //variable declaration
    /*index val; i, j;
    //diameter...;
    //filter size: maxk
    //img size: size*/
    int i, j, diameter, maxk, size;

    //Effective guarantee of variable
    if(sigma_color <= 0)
        sigma_color = 1;
    if(sigma_space <= 0)
        sigma_space = 1;

    //weight denominator pre calculation
    double gauss_color_coeff = -0.5/(sigma_color * sigma_color)/(cn * cn);
    double gauss_space_coeff = -0.5/(sigma_space * sigma_space);

    //Assign more bit width to more sensitive weight
    //uint32_t smax = (uint32_t)(pow(2,(10))-1);
    //uint32_t cmax = (uint32_t)(pow(2,(14)));
    //uint32_t max_chk = smax * cmax * 256;
    //printf("maximu=%ud\n",max_chk);

    //Effective guarantee of variable
    if(radius <= 0)
        radius = (int)(sigma_space*1.5);

    radius = radius>1 ? radius: 1;

    diameter = radius*2 + 1;

    //Expand the boundary to eliminate the branching judgement
    //of the boundary in the subsequent calculation
    /*
    Mat_t tmp;
    copyMakeBorder( src, &tmp, radius);*/

    //initialize Look-Up-Table
    //The weight table provides the weight of the filtering formular;
    //The offest table provides offsets of other pixels in the window with
    //the center point as the base address.
    uint32_t * color_weight = (uint32_t*)malloc(256*cn*sizeof(uint32_t));
    uint32_t * space_weight = (uint32_t*)malloc(diameter * diameter * sizeof(uint32_t));
    int * space_ofs = (int*)malloc(diameter * diameter * sizeof(int));
    //printf("Color_weight:\n");
    for(i=0; i<256 *cn; i++){
        //The maximum of pixel val is 255;
        //Thus, the maximum of weight = sqrt(2^32/256)
        color_weight[i] = (uint32_t)(16384 * exp(i*i*gauss_color_coeff));
        //printf("cw[%d]=%d\n",i, color_weight[i]);
    }

    //printf("Space_weight:\n");
    for(i=-radius, maxk = 0; i <= radius; i++)
    {
        j = -radius;

        for(; j <= radius; j++)
        {
            double r = sqrt((double)i*i + (double) j*j);

            if(r > radius)
                continue;
            space_weight[maxk] = (uint32_t)(16384 * exp(r*r*gauss_space_coeff));
            //printf("sw[%d]=%d\n",maxk, space_weight[maxk]);
            space_ofs[maxk++] = (int)(i * tmp->width*cn + j*cn);
        }
    }

    bilateralFilterInvoker_fixed(dst, tmp, radius, maxk, space_ofs, space_weight, color_weight);

    free(color_weight);
    free(space_weight);
    free(space_ofs);
}

/*Separated Bilateral Filter accelerated by losing performance
//Maximum Image Width Support: 16777 pixels
*/



void bilateralFilter_simd(Mat_t* _src, Mat_t* _dst, int radius,
                     double sigmaColor, double sigmaSpace, uint8_t tag)
{
    copy_MatHead(_src, _dst);

    switch(tag){
    case 0:
    bilateralFilter_float(_src, _dst, radius, sigmaColor, sigmaSpace);
    break;
    case 1:
    bilateralFilter_fixed(_src, _dst, radius, sigmaColor, sigmaSpace);
    break;
    default:
    break;
    }

}

void bilateralFilter_simd_beta(Mat_t* _src, Mat_t* _dst, int radius,
                           double sigmaColor, double sigmaSpace)
{
    bilateralFilter_fixed_beta(_src, _dst, radius, sigmaColor, sigmaSpace);
}
#endif

/*Fraction Part;;; 
//ADDITION DATE: Mar23rd*/
void bilateralFilter_fraction(Mat_t* src, Mat_t* dst, int radius,
                        double sigma_color, double sigma_space)
{
    //channel size. At present, there are only two cases:
    //1 and 3
    int cn = src->cn;
    //variable declaration
    /*index val; i, j;
    //diameter...;
    //filter size: maxk
    //img size: size*/
    int i, j, diameter, maxk, size;

    //Effective guarantee of variable
    if(sigma_color <= 0)
        sigma_color = 1;
    if(sigma_space <= 0)
        sigma_space = 1;

    //weight denominator pre calculation
    double gauss_color_coeff = -0.5/(sigma_color * sigma_color)/(cn * cn);
    double gauss_space_coeff = -0.5/(sigma_space * sigma_space);

    //Assign more bit width to more sensitive weight
    //uint32_t smax = (uint32_t)(pow(2,(10))-1);
    //uint32_t cmax = (uint32_t)(pow(2,(14)));
    //uint32_t max_chk = smax * cmax * 256;
    //printf("maximu=%ud\n",max_chk);

    //Effective guarantee of variable
    if(radius <= 0)
        radius = (int)(sigma_space*1.5);

    radius = radius>1 ? radius: 1;

    diameter = radius*2 + 1;

    //Expand the boundary to eliminate the branching judgement
    //of the boundary in the subsequent calculation
    Mat_t tmp;
    copyMakeBorder( src, &tmp, radius);

    size = dst->width * dst->height * cn;
    if(cn == 1){
        dst->data.gray = (uint8_t*)malloc(size*sizeof(uint8_t));
        memset(dst->data.gray, 0, size);
    }else{
        dst->data.rgb = (rgb_t*)malloc(size*sizeof(uint8_t));
        memset(dst->data.rgb, 0, size);
    }

    //initialize Look-Up-Table
    //The weight table provides the weight of the filtering formular;
    //The offest table provides offsets of other pixels in the window with
    //the center point as the base address.
    fraction * color_weight = (fraction*)malloc(256*cn*sizeof(fraction));
    fraction * space_weight = (fraction*)malloc(diameter * diameter * sizeof(fraction));
    int * space_ofs = (int*)malloc(diameter * diameter * sizeof(int));
    //printf("Color_weight:\n");
    for(i=0; i<256 *cn; i++){
        //The maximum of pixel val is 255;
        //Thus, the maximum of weight = sqrt(2^32/256)
         d2fr(exp(i*i*gauss_color_coeff), &color_weight[i]);
        //printf("cw[%d]=%d\n",i, color_weight[i]);
    }

    //printf("Space_weight:\n");
    for(i=-radius, maxk = 0; i <= radius; i++)
    {
        j = -radius;

        for(; j <= radius; j++)
        {
            double r = sqrt((double)i*i + (double) j*j);

            if(r > radius)
                continue;

            d2fr(exp(r*r*gauss_space_coeff),&space_weight[maxk]);
            //printf("sw[%d]=%d\n",maxk, space_weight[maxk]);
            space_ofs[maxk++] = (int)(i * tmp.width*cn + j*cn);
        }
    }

    bilateralFilterInvoker_fraction(dst, &tmp, radius, maxk, space_ofs, space_weight, color_weight);

    free(color_weight);
    free(space_weight);
    free(space_ofs);
}