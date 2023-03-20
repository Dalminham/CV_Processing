#ifndef _INCLUDE_PIXEL_FORMAT_H
#define _INCLUDE_PIXEL_FORMAT_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BAND_DATA_SIZE_1200DPI    (400*1024)
#define BAND_SIZE                 sizeof(band_plane_t)

typedef struct rgb_plane
{
    uint8_t *r;
    uint8_t *g;
    uint8_t *b;
}rgb_plane_t;

typedef union
{
    uint16_t *raw16;
    uint8_t *raw8;
    rgb_plane_t rgb;
    uint8_t *gray;
}data_u;

typedef struct band_plane
{
    data_u  data;           //band数据
    int32_t width;          //band宽
    int32_t height;         //band高
    // int32_t line_width;     //band一行的大小
    int32_t pos;            //当前band在整张图像的位置信息
    uint8_t bit_depth;      //当前band的色深
    uint8_t is_gray;    //标志当前band是否为灰度图，默认为彩色图

    uint32_t image_width;
    uint32_t image_height;

    struct band_plane *pre_band;    //前一个band指针
    struct band_plane *next_band;   //后一个band指针

}band_plane_t;

typedef struct image
{
    int32_t width;        //图像宽
    int32_t height;       //图像高
    uint8_t bit_depth;    //图像的色深
    uint8_t is_gray;      //标志当前图像是否为灰度图，默认为彩色图

    band_plane_t *head_band;    //图像band链表的头指针
    band_plane_t *tail_band;    //图像band链表的尾指针
}image_t;

#endif//_INCLUDE_PIXEL_FORMAT_H

