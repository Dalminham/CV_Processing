#include "cv_utils.h"

//Mirror image to copy image boundary symmetrically,
//with adaptive parameters
void copyMakeBorder(Mat_t *src, Mat_t *dst, int radius)
{
    int width, height, cn, new_width, new_height, new_size, bit_width;
    width = src->width;
    height = src->height;
    cn = src->cn;
    new_width = width + 2 * radius;
    new_height = height + 2 * radius;
    dst->width = new_width;
    dst->height = new_height;
    dst->cn = cn;
    bit_width = cn * width;
    new_size = new_width*new_height;

    //radius range check, it can not be bigger than img_w
    if(radius > width)
    {
        printf("[%s] Error! radius must be less than img width!\n",__func__);
        exit(0);
    }
    //memory manage
    if(cn == 1)
    {
        dst->data.gray = (uint8_t*)malloc(new_size * sizeof(uint8_t));
        memset(dst->data.gray, 0, new_size*sizeof(uint8_t));
    }else{
        dst->data.rgb = (rgb_t*)malloc(new_size * sizeof(rgb_t));
        memset(dst->data.rgb,0, new_size*sizeof(rgb_t));
    }

    //Border Maker
    if(cn == 1)
    {
        //parallelize，including body copy
        for(uint32_t i=0; i<height;i++){
            uint8_t * LocalLine = src->data.gray + i*width;
            uint8_t * DstLine = dst->data.gray + (i+radius)*new_width;
            uint32_t j=0, k;
            for(;j<radius;j++)
            {
                DstLine[j]= LocalLine[radius-j];
            }
            k=0;
            for(; k< width;k++,j++)
            {
                DstLine[j]= LocalLine[k];
            }
            for(;k<width+radius;k++,j++)
            {
                DstLine[j]=LocalLine[2*width-k-2];
            }
        }
        //vertical, only border
        for(int i=0; i<new_width;i++){
            uint8_t * HLocalLine = dst->data.gray + radius*new_width;
            uint8_t * TLocalLine = dst->data.gray + (radius+height-2)*new_width;
            uint8_t * HeadLine = dst->data.gray;
            uint8_t * TailLine = dst->data.gray + (height + radius)*new_width;
            for(int j = 0;j<radius;j++)
            {
                HeadLine[i + j*new_width] = HLocalLine[i+ (radius-j)*new_width];
                TailLine[i + j*new_width] = TLocalLine[(-j)*new_width+i];
            }
        }
    }else
    {
        //parallelize, including body copy
        rgb_t * LocalLine = src->data.rgb;
        rgb_t * DstLine = dst->data.rgb + radius*new_width;
        for(uint32_t i = 0; i < height; i++)
        {
            uint32_t j=0,k;
            for(;j<radius;j++)
            {
                DstLine[i*new_width+j].r = LocalLine[i*width+radius-j].r;
                DstLine[i*new_width+j].g = LocalLine[i*width+radius-j].g;
                DstLine[i*new_width+j].b = LocalLine[i*width+radius-j].b;
            }
            k=0;
            for(; k< width;k++,j++)
            {
                DstLine[i*new_width+j].r= LocalLine[i*width+k].r;
                DstLine[i*new_width+j].g= LocalLine[i*width+k].g;
                DstLine[i*new_width+j].b= LocalLine[i*width+k].b;
            }
            for(;k<width+radius;k++,j++)
            {
                DstLine[i*new_width+j].r=LocalLine[i*width+2*width-k-2].r;
                DstLine[i*new_width+j].g=LocalLine[i*width+2*width-k-2].g;
                DstLine[i*new_width+j].b=LocalLine[i*width+2*width-k-2].b;
            }
        }
        //vertical, only border
        rgb_t * HLocalLine = dst->data.rgb + radius*new_width;
        rgb_t * TLocalLine = dst->data.rgb + (radius+height-2)*new_width;
        rgb_t * HeadLine = dst->data.rgb;
        rgb_t * TailLine = dst->data.rgb + (radius+height)*new_width;
        for(int i=0; i<new_width;i++)
        {
            for(int j=0;j<radius;j++)
            {
                HeadLine[i + j*new_width].r = HLocalLine[i+ (radius-j)*new_width].r;
                HeadLine[i + j*new_width].g = HLocalLine[i+ (radius-j)*new_width].g;
                HeadLine[i + j*new_width].b = HLocalLine[i+ (radius-j)*new_width].b;
                TailLine[i + j*new_width].r = TLocalLine[(-j)*new_width+i].r;
                TailLine[i + j*new_width].g = TLocalLine[(-j)*new_width+i].g;
                TailLine[i + j*new_width].b = TLocalLine[(-j)*new_width+i].b;
            }
        }
    }
}

//Check two bmp files' img_data similar or not
float bmp_similar(uint8_t* src1, uint8_t* src2, uint32_t size)
{
    uint32_t cnt = 0;
    for(uint32_t i=0;i<size;i++)
    {
        if(src1[i] != src2[i])
        cnt++;
    }
    return 1.0 - cnt/size;
}