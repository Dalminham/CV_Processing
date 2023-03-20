#ifndef _INCLUDE_BMP_H
#define _INCLUDE_BMP_H


#include <sys/time.h>

#include "format.h"
#include "algorithm.h"
/*
 * 打印调试信息
 */
//#define DEBUG_SWITCH
#ifdef  DEBUG_SWITCH
#define pr_debug(fmt,args...) printf(fmt, ##args)
#else
#define pr_debug(fmt,args...) /* do nothing */
#endif

// 用于使图像宽度所占字节数为4byte的倍数
#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)

#define UINT8_LIMIT(a) if(a > 255){ a = 255;}else if(a < 0){a = 0;}

/* bmp位图文件头 */
typedef struct
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t off_bits;
}__attribute__ ((packed)) bmp_file_header_t;

/* bmp位图信息头 */
typedef struct
{
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    uint32_t x_pels_permeter;
    uint32_t y_pels_permeter;
    uint32_t clr_used;
    uint32_t clr_important;
} bmp_info_header_t;

/* bmp图像 */
typedef struct
{
    bmp_file_header_t bmp_file_header;
    bmp_info_header_t bmp_info_header;

    /* 调色板数据 */
    uint8_t palette[1024];

    /* 位图数据指针 */
    uint8_t *bmp_data;
}BMP;


static inline uint8_t saturated_add_bu(uint8_t a, uint8_t b)
{
    uint8_t result;
    uint16_t tmp;

    tmp = a + b;

    if (tmp > 255)
        result = 255;
    else
        result = tmp;

    return result;
}



int32_t read_bmp(const char *file_path, BMP *bmp);
int32_t save_bmp(const char *file_path, BMP *bmp);
int32_t bmp2plane(BMP *bmp, image_t *image);
void plane2bmp(image_t *image, uint8_t *bmp_data);
void insert_image(image_t *image, band_plane_t *band);
void free_band(band_plane_t *band);

void init_bilateral_table(double sigma_s, double sigma_r);
int32_t bilateralFilter(band_plane_t *band, int32_t window_size, int loop_num);
band_plane_t* operator_image(band_plane_t *band, int radius, int32_t image_height);

band_plane_t *copy_band(band_plane_t *band);
void print_LUT(uint8_t radius);
void *memcpy_custom (void *__restrict, const void *__restrict, size_t);
#endif //_INCLUDE_BMP_H

