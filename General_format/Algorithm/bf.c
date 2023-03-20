#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../Utils/bmp.h"

#define DISTANCE (7)
/* 最大支持半径 7， 最大支持直径15 */
static double table_sigma_r[256];       //中心点位和遍历点的像素域表， 大小为像素值范围
static double table_sigma_s[16];         //中心点位和遍历点的空间域表, 大小为半径

static uint64_t table_w[DISTANCE][256];

static uint8_t tmp_r[BAND_DATA_SIZE_1200DPI] = {0};
static uint8_t tmp_g[BAND_DATA_SIZE_1200DPI] = {0};
static uint8_t tmp_b[BAND_DATA_SIZE_1200DPI] = {0};
static uint8_t new_r[BAND_DATA_SIZE_1200DPI] = {0};
static uint8_t new_g[BAND_DATA_SIZE_1200DPI] = {0};
static uint8_t new_b[BAND_DATA_SIZE_1200DPI] = {0};
static uint8_t *tmp_gray = tmp_r;
static uint8_t *new_gray = new_r;

static band_plane_t bilateral_save_band = {0};

void init_bilateral_table(double sigma_s, double sigma_r)
{
    double w_sigma_s = 2.0f * sigma_s * sigma_s;
    double w_sigma_r = 2.0f * sigma_r * sigma_r;

    for(uint32_t i = 0; i < 256; i++)
    {
        table_sigma_r[i] = exp(-(1.0f) * (pow((double)i, 2) / (w_sigma_r)));
    }

    for(uint32_t i = 0; i < DISTANCE; i++)
    {
        table_sigma_s[i] = exp(-(1.0f) * (i * i) / (w_sigma_s));
    }

    for(uint32_t i = 0; i < DISTANCE; i++)
    {
        for (uint32_t j = 0; j < 256; j++)
        {
            table_w[i][j] = (uint32_t)(table_sigma_s[i] * table_sigma_r[j] * 10000);
        }
    }
}

int32_t bilateralFilter(band_plane_t *band, int32_t window_size, int loop_num)
{
    //判断band是否有效
    if(0 == band->width || 0 == band->height)
    {
        return SUCCESS;
    }

    int32_t width = band->width;
    int32_t height = band->height;

    uint8_t *gray = NULL;
    uint8_t *r = NULL;
    uint8_t *g = NULL;
    uint8_t *b = NULL;

    if (band->is_gray == 1)
    {
        gray = band->data.gray;
    }
    else
    {
        r = band->data.rgb.r;
        g = band->data.rgb.g;
        b = band->data.rgb.b;
    }

    pr_debug("[%s] width = %d\n", __func__, width);
    pr_debug("[%s] height = %d\n", __func__, height);
    pr_debug("[%s] image_height = %d\n", __func__, band->image_height);
    pr_debug("[%s] window_size = %d\n", __func__, window_size);

    if (height < (window_size * 2))
    {        
        //printf("window_size !!! height = %d\n", height);
        error_num = ERROR_ILLEGAL_PARAMETERS;
        return FAILED;
    }
    
    /* 直径 */
    int32_t diameter = window_size * 2 + 1;
    if (diameter > 32)
    {
        //printf("[%s] window_size is too large !!!!\n", __func__);
        error_num = ERROR_ILLEGAL_PARAMETERS;
        return FAILED;
    }

    pr_debug("[%s] band->pos = %d\n", __func__, band->pos);
    /* 判断是否为图像的最后一块 */
    uint8_t last_band_flags = 0;
    if (band->image_height == (band->pos + height))
    {
        last_band_flags = 1;
    }

    /* 计算band需要保留的行数（下一个band需要使用的函数） */
    int32_t row_num = window_size;
    pr_debug("[%s] row_num = %d\n", __func__, row_num);

    /* 计算目标band的高 */
    int32_t dst_width = width;
    int32_t dst_height;
    if (last_band_flags)
    {
        dst_height = height + row_num;
    }
    else if(band->pos == 0)
    {
        dst_height = height - row_num;
    }
    else
    {
        dst_height = height;
    }

    pr_debug("[%s] dst_width = %d\n", __func__, dst_width);
    pr_debug("[%s] dst_height = %d\n", __func__, dst_height);

    uint8_t *dst_r = band->data.rgb.r;
    uint8_t *dst_g = band->data.rgb.g;
    uint8_t *dst_b = band->data.rgb.b;

    uint8_t *dst_gray = band->data.gray;

    if (band->is_gray == 1)
    {
        pr_debug("[%s][%d]\n", __func__, __LINE__);

        /* 如果不是第一个band，则数据加上前一个band的保留的行 */
        uint8_t *pre_gray;

        if (band->pos != 0 )
        {
            band_plane_t *pre_band = &bilateral_save_band;
            uint32_t offset = pre_band->width * (pre_band->height - row_num * 2);
            pre_gray = pre_band->data.gray;

            memcpy(new_gray, pre_gray + offset, width * row_num * 2);
            memcpy(new_gray + width * row_num * 2, gray, width * height);

            /* memcpy(tmp_gray, new_gray, width * (height +row_num * 2)); */
        }
        else
        {
            memcpy(new_gray, gray, width * height);

            /* memcpy(tmp_gray, new_gray, width * height); */
        }
        pr_debug("[%s][%d]\n", __func__, __LINE__);
    }
    else
    {
        /* 如果不是第一个band，则数据加上前一个band的最后一行 */
        uint8_t *pre_r;
        uint8_t *pre_g;
        uint8_t *pre_b;

        if (band->pos != 0 )
        {
            band_plane_t *pre_band = &bilateral_save_band;
            uint32_t offset = pre_band->width * (pre_band->height - row_num * 2);
            pre_r = pre_band->data.rgb.r;
            pre_g = pre_band->data.rgb.g;
            pre_b = pre_band->data.rgb.b;

            memcpy(new_r, pre_r + offset, width * row_num * 2);
            memcpy(new_g, pre_g + offset, width * row_num * 2);
            memcpy(new_b, pre_b + offset, width * row_num * 2);
            memcpy(new_r + width * row_num * 2, r, width * height);
            memcpy(new_g + width * row_num * 2, g, width * height);
            memcpy(new_b + width * row_num * 2, b, width * height);
            
            /* memcpy(tmp_r, new_r, width * (height +row_num * 2)); */
            /* memcpy(tmp_g, new_g, width * (height +row_num * 2)); */
            /* memcpy(tmp_b, new_b, width * (height +row_num * 2)); */
        }
        else
        {
            memcpy(new_r, r, width * height);
            memcpy(new_g, g, width * height);
            memcpy(new_b, b, width * height);

            /* memcpy(tmp_r, new_r, width * height); */
            /* memcpy(tmp_g, new_g, width * height); */
            /* memcpy(tmp_b, new_b, width * height); */
        }
    }

    //备份当前处理的原band数据，用于下一个band的处理
    memcpy(&bilateral_save_band, band, BAND_SIZE);
    
    memset((void *)dst_r, 0, dst_height * dst_width);
    memset((void *)dst_g, 0, dst_height * dst_width);
    memset((void *)dst_b, 0, dst_height * dst_width);

    /* 如果是第一块开始的行高在0行，否则开始行高在2行 */
    int32_t height_start, height_end;
    if (band->pos == 0)
    {
        height_start = 0;
        height_end = dst_height;
    }
    else
    {
        height_start = window_size;
        height_end = dst_height + window_size;
    }
    pr_debug("[%s] height_start = %d\n", __func__, height_start);
    pr_debug("[%s] height_end = %d\n", __func__, height_end);


    //printf("loop_num=%d\n", loop_num);
    for(int cc = 0; cc < loop_num; cc++){
    /* 双边滤波算法 */
    //垂直
    for (int32_t j = 0; j < dst_width; j++)
    {
        for (int32_t i = height_start; i < height_end; i++)
        {
            uint64_t fenzi_b, fenzi_g, fenzi_r, fenzi_gray;
            uint64_t fenmu_b, fenmu_g, fenmu_r, fenmu_gray;
 
            fenzi_b = fenzi_g = fenzi_r = fenzi_gray = 0;
            fenmu_b = fenmu_g = fenmu_r = fenmu_gray = 0;
 
            /* 计算中心点 */
            int32_t cur_point = dst_width * i + j;
 
            for (int32_t m = i - window_size; m <= i + window_size; m++)
            {
                if ( !(m < 0) )
                {
                    if(last_band_flags == 1 && (m > height_end - 1))
                        break;
 
                    /* 当前卷积核点 */
                    int32_t win_point = dst_width * m + j;
 
                    if (band->is_gray == 1)
                    {
                        uint64_t w_w_gray = table_w[abs(m-i)][abs(new_gray[cur_point] - new_gray[win_point])];
                        fenmu_gray += w_w_gray;
                        fenzi_gray += w_w_gray * new_gray[win_point];
                    }
                    else
                    {
                        uint64_t w_w_b = table_w[abs(m-i)][abs(new_b[cur_point] - new_b[win_point])];
                        uint64_t w_w_g = table_w[abs(m-i)][abs(new_g[cur_point] - new_g[win_point])];
                        uint64_t w_w_r = table_w[abs(m-i)][abs(new_r[cur_point] - new_r[win_point])];
                        fenmu_b += w_w_b;
                        fenmu_g += w_w_g;
                        fenmu_r += w_w_r;
                        fenzi_b += w_w_b * (new_b[win_point]);
                        fenzi_g += w_w_g * (new_g[win_point]);
                        fenzi_r += w_w_r * (new_r[win_point]);
                    }
                }
            }

            /* 计算中心点和当前卷积核点 */
            if (band->is_gray == 1)
            {
                tmp_gray[cur_point] = (uint8_t)(fenzi_gray / fenmu_gray);
            }
            else
            {
                tmp_b[cur_point] = (uint8_t)(fenzi_b / fenmu_b);
                tmp_g[cur_point] = (uint8_t)(fenzi_g / fenmu_g);
                tmp_r[cur_point] = (uint8_t)(fenzi_r / fenmu_r);
            }
        }
    }
    
    //水平
    for (int32_t i = height_start; i < height_end; i++)
    {
        for (int32_t j = 0; j < dst_width; j++)
        {
            uint64_t fenzi_b, fenzi_g, fenzi_r, fenzi_gray;
            uint64_t fenmu_b, fenmu_g, fenmu_r, fenmu_gray;
 
            fenzi_b = fenzi_g = fenzi_r = fenzi_gray = 0;
            fenmu_b = fenmu_g = fenmu_r = fenmu_gray = 0;

            /* 计算中心点 */
            int32_t cur_point = dst_width * i + j;
 
            for (int32_t n = j - window_size; n <= j + window_size; n++)
            {
                if ( !(n < 0 || n > dst_width -1) )
                {
                    /* 当前卷积核点 */
                    int32_t win_point = dst_width * i + n;
 
                    if (band->is_gray == 1)
                    {
                        uint64_t w_w_gray = table_w[abs(n-j)][abs(tmp_gray[cur_point] - tmp_gray[win_point])];
                        fenmu_gray += w_w_gray;
                        fenzi_gray += w_w_gray * tmp_gray[win_point];
                    }
                    else
                    {
                        uint64_t w_w_b = table_w[abs(n-j)][abs(tmp_b[cur_point] - tmp_b[win_point])];
                        uint64_t w_w_g = table_w[abs(n-j)][abs(tmp_g[cur_point] - tmp_g[win_point])];
                        uint64_t w_w_r = table_w[abs(n-j)][abs(tmp_r[cur_point] - tmp_r[win_point])];
                        fenmu_b += w_w_b;
                        fenmu_g += w_w_g;
                        fenmu_r += w_w_r;
                        fenzi_b += w_w_b * (tmp_b[win_point]);
                        fenzi_g += w_w_g * (tmp_g[win_point]);
                        fenzi_r += w_w_r * (tmp_r[win_point]);
                    }
                }
            }

            /* 计算中心点和当前卷积核点 */
            int32_t dst_cur_point = dst_width * (i - height_start) + j;
            if (band->is_gray == 1)
            {
                dst_gray[dst_cur_point] = (uint8_t)(fenzi_gray / fenmu_gray);
            }
            else
            {
                dst_b[dst_cur_point] = (uint8_t)(fenzi_b / fenmu_b);
                dst_g[dst_cur_point] = (uint8_t)(fenzi_g / fenmu_g);
                dst_r[dst_cur_point] = (uint8_t)(fenzi_r / fenmu_r);
            }
        }
    }
    }
    band->width = dst_width;
    band->height = dst_height;
    if (band->pos == 0)
    {
        band->pos = 0;
    }
    else
    {
        band->pos = band->pos - row_num;
    }
    pr_debug("[%s] band->pos = %d\n", __func__, band->pos);

    return SUCCESS;
}
