#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "bmp.h"

int32_t read_bmp(const char *file_path, BMP *bmp)
{
    FILE *file = NULL;
    uint32_t line_width = 0;
    int32_t width = 0;
    int32_t height = 0;
    uint32_t s_bmp_data_offset = 0;
    int32_t err = 0;
    int i = 0;

    uint8_t **s_bmp_data = &(bmp->bmp_data);
    pr_debug("[%s] bmp->bmp_data = %p\n", __func__, bmp->bmp_data);
    pr_debug("[%s] *s_bmp_data = %p\n", __func__, *s_bmp_data);
    bmp_file_header_t *s_bmp_file_header = &(bmp->bmp_file_header);
    bmp_info_header_t *s_bmp_info_header = &(bmp->bmp_info_header);

    do {
        if (NULL == file_path)
        {
            err = -1;
            break;
        }
        pr_debug("[%s] file_path = %s\n", __func__, file_path);

        file = fopen(file_path, "rb");
        if (NULL == file)
        {
            err = -1;
            break;
        }

        /* 读取位图文件头 */
        fread(s_bmp_file_header, sizeof(bmp_file_header_t), 1, file);

        /* 读取位图信息头 */
        fread(s_bmp_info_header, sizeof(bmp_info_header_t), 1, file);

        pr_debug("[%s] s_bmp_file_header->type = 0x%x\n", __func__, s_bmp_file_header->type);
        pr_debug("[%s] s_bmp_file_header->size = 0x%x\n", __func__, s_bmp_file_header->size);
        pr_debug("[%s] s_bmp_file_header->off_bits = 0x%x\n", __func__, s_bmp_file_header->off_bits);
        pr_debug("[%s] s_bmp_info_header->size = 0x%x\n", __func__, s_bmp_info_header->size);
        pr_debug("[%s] s_bmp_info_header->width = 0x%x\n", __func__, s_bmp_info_header->width);
        pr_debug("[%s] s_bmp_info_header->height = 0x%x\n", __func__, s_bmp_info_header->height);
        pr_debug("[%s] s_bmp_info_header->planes = 0x%x\n", __func__, s_bmp_info_header->planes);
        pr_debug("[%s] s_bmp_info_header->bit_count = 0x%x\n", __func__, s_bmp_info_header->bit_count);
        pr_debug("[%s] s_bmp_info_header->compression = 0x%x\n", __func__, s_bmp_info_header->compression);
        pr_debug("[%s] s_bmp_info_header->size_image = 0x%x\n", __func__, s_bmp_info_header->size_image);
        pr_debug("[%s] s_bmp_info_header->x_pels_permeter = 0x%x\n", __func__, s_bmp_info_header->x_pels_permeter);
        pr_debug("[%s] s_bmp_info_header->y_pels_permeter = 0x%x\n", __func__, s_bmp_info_header->y_pels_permeter);
        pr_debug("[%s] s_bmp_info_header->clr_used = 0x%x\n", __func__, s_bmp_info_header->clr_used);
        pr_debug("[%s] s_bmp_info_header->clr_important = 0x%x\n", __func__, s_bmp_info_header->clr_important);

        /* 如果存在调色板，则读取调色板，常规的bmp文件头加信息头大小为0x36 */
        if (s_bmp_file_header->off_bits != 0x36)
        {
            fread(bmp->palette, s_bmp_file_header->off_bits - 0x36, 1, file);
        }

        /* 读取图片的高宽 */
        width = s_bmp_info_header->width;
        height = s_bmp_info_header->height;

        /* 通过像素数/比特数计算图像的实际宽度 */
        width = s_bmp_info_header->width;
        line_width = WIDTHBYTES(width * s_bmp_info_header->bit_count);

        pr_debug("[%s] line_width = %d, width = %d, height = %d\n", __func__, line_width, width, height);

        /* 分配位图数据空间 */
        *s_bmp_data = (uint8_t *)malloc(line_width * height);
        pr_debug("[%s] *s_bmpdata = %p\n", __func__, *s_bmp_data);

        /* 读取位图数据的偏移值，并重定位文件的偏移量 */
        s_bmp_data_offset = s_bmp_file_header->off_bits;
        fseek(file, s_bmp_data_offset, SEEK_SET);
        //s_bmp_file_header->off_bits = 0x36;

        /* 读取位图数据 */
        for (i = height - 1; i >= 0; i--)
        {
            fread(*s_bmp_data + i * line_width, line_width, 1, file);
        }

    } while (0);

    if (file != NULL)
    {
        fclose(file);
    }
    return err;
}

int32_t save_bmp(const char *file_path, BMP *bmp)
{
    FILE *file = NULL;
    uint32_t line_width = 0; 
    uint32_t width = 0;
    uint32_t height = 0;
    /* uint8_t alpha = 0; */
    int32_t err = 0;
    int32_t i;

    uint8_t **s_bmp_data = &(bmp->bmp_data);
    pr_debug("[%s] bmp->bmp_data = %p\n", __func__, bmp->bmp_data);
    bmp_file_header_t *s_bmp_file_header = &(bmp->bmp_file_header);
    bmp_info_header_t *s_bmp_info_header = &(bmp->bmp_info_header);
    uint8_t* palette = bmp->palette;

    do {
        if (NULL == file_path)
        {
            err = -1;
            break;
        }

        file = fopen(file_path, "wb");
        if (NULL == file)
        {
            err = -1;
            break;
        }

        /* 调整位图信息头的大小，标准信息头大小为0x28 */
        s_bmp_info_header->size = 0x28;



        /* 填充位图文件头 */
        fwrite(s_bmp_file_header, 1, sizeof(bmp_file_header_t), file);

        /* 填充位图信息头 */
        fwrite(s_bmp_info_header, 1, sizeof(bmp_info_header_t), file);

        /* 填充调色板 */
        if (s_bmp_file_header->off_bits != 0x36)
        {
            fwrite(palette, 1,  s_bmp_file_header->off_bits - 0x36, file);
        }

        pr_debug("[%s] s_bmp_file_header->type = 0x%x\n", __func__, s_bmp_file_header->type);
        pr_debug("[%s] s_bmp_file_header->size = 0x%x\n", __func__, s_bmp_file_header->size);
        pr_debug("[%s] s_bmp_file_header->off_bits = 0x%x\n", __func__, s_bmp_file_header->off_bits);
        pr_debug("[%s] s_bmp_info_header->size = 0x%x\n", __func__, s_bmp_info_header->size);
        pr_debug("[%s] s_bmp_info_header->width = 0x%x\n", __func__, s_bmp_info_header->width);
        pr_debug("[%s] s_bmp_info_header->height = 0x%x\n", __func__, s_bmp_info_header->height);
        pr_debug("[%s] s_bmp_info_header->planes = 0x%x\n", __func__, s_bmp_info_header->planes);
        pr_debug("[%s] s_bmp_info_header->bit_count = 0x%x\n", __func__, s_bmp_info_header->bit_count);
        pr_debug("[%s] s_bmp_info_header->compression = 0x%x\n", __func__, s_bmp_info_header->compression);
        pr_debug("[%s] s_bmp_info_header->size_image = 0x%x\n", __func__, s_bmp_info_header->size_image);
        pr_debug("[%s] s_bmp_info_header->x_pels_permeter = 0x%x\n", __func__, s_bmp_info_header->x_pels_permeter);
        pr_debug("[%s] s_bmp_info_header->y_pels_permeter = 0x%x\n", __func__, s_bmp_info_header->y_pels_permeter);
        pr_debug("[%s] s_bmp_info_header->clr_used = 0x%x\n", __func__, s_bmp_info_header->clr_used);
        pr_debug("[%s] s_bmp_info_header->clr_important = 0x%x\n", __func__, s_bmp_info_header->clr_important);

        /* 通过像素数/比特数计算图像的实际宽度 */
        width = s_bmp_info_header->width;
        line_width = WIDTHBYTES(width * s_bmp_info_header->bit_count);
        height = s_bmp_info_header->height;

        pr_debug("[%s] line_width = %d, width = %d, height = %d\n", __func__, line_width, width, height);

        /* 填充位图数据 */
        for (i = height - 1; i >= 0; i--)
        {
            int err = fwrite(*s_bmp_data + i * line_width, 1, line_width, file);
        }

        fflush(file);
    } while (0);

    free(bmp->bmp_data);

    if (file != NULL)
    {
        fclose(file);
    }

    return err;
}

int32_t bmp2plane(BMP *bmp, image_t *image)
{
    uint8_t **s_bmp_data = &(bmp->bmp_data);
    /* bmp_file_header_t *s_bmp_file_header = &(bmp->bmp_file_header); */
    bmp_info_header_t *s_bmp_info_header = &(bmp->bmp_info_header);

    /* 获取原位图数据高宽，并计算数据实际的宽度（4字节对齐） */
    int32_t width = s_bmp_info_header->width;
    int32_t height = s_bmp_info_header->height;
    int32_t line_width = WIDTHBYTES(width * s_bmp_info_header->bit_count);

    /* 计算每个像素点的字节数 */
    uint8_t width_p = s_bmp_info_header->bit_count / 8;

    uint8_t is_gray = 0;
    if (width_p <= 1)
    {
        is_gray = 1;
    }

    image->width = s_bmp_info_header->width;
    image->height = s_bmp_info_header->height;
    image->bit_depth = s_bmp_info_header->bit_count;
    image->is_gray = is_gray;
    image->head_band = NULL;
    image->tail_band = NULL;
    
    /*add by wyj*/
    int16_t band_num = 0;
    for (int32_t i = 0; i < height; i += 32)
    {
        int32_t counter = 0;
        int32_t band_height;
        band_plane_t *band;
        band = (band_plane_t *)malloc(sizeof(band_plane_t));
        memset((void *)band, 0, sizeof(band_plane_t));
        // printf("+++++[%s][%d][i:%d]+++++\r\n", __FUNCTION__, __LINE__, i);

        insert_image(image, band);

        if (is_gray == 1)
        {
            band->data.gray = (uint8_t*)malloc(32 * width);
            band->is_gray = 1;
        }
        else
        {
            band->data.rgb.r = (uint8_t*)malloc(32 * width);
            band->data.rgb.g = (uint8_t*)malloc(32 * width);
            band->data.rgb.b = (uint8_t*)malloc(32 * width);
            band->is_gray = 0;
        }

        if ((height - i) < 32)
        {
            band_height = height - i;
        }
        else
        {
            band_height = 32;
        }

        /* band处理 */
        for (int32_t k = 0; k < band_height; k++)
        {
            // printf("+++++[%s][%d][k:%d]+++++\r\n", __FUNCTION__, __LINE__, k);
            if (is_gray == 1)
            {
                int32_t real_width = (width / (8 / s_bmp_info_header->bit_count));
                int32_t line_width = WIDTHBYTES(width * s_bmp_info_header->bit_count);

                /* pr_debug("[%s] band k = %d\n", __func__, k); */
                /* pr_debug("[%s] real_width = %d\n", __func__, real_width); */
                /* pr_debug("[%s] line_width = %d\n", __func__, line_width); */

                /* pr_debug("[%s] bmp_data = %p\n", __func__, bmp_data); */
                /* pr_debug("[%s] band->data.gray = %p\n", __func__, band->data.gray); */

                uint8_t *bmp_data_point = *s_bmp_data + (i + k) * line_width;
                uint8_t *gray_point = band->data.gray + k * real_width;

                /* pr_debug("[%s] bmp_data_point = %p\n", __func__, bmp_data_point); */
                /* pr_debug("[%s] gray_point = %p\n", __func__, gray_point); */
                memcpy((void *)gray_point, (void *)bmp_data_point, real_width);
            }
            else
            {
                for (int32_t j = 0; j < width; j++)
                {
                    int32_t pixel_point = (i + k) * line_width + j * width_p;

                    band->data.rgb.b[counter] = (*s_bmp_data)[pixel_point + 0];
                    band->data.rgb.g[counter] = (*s_bmp_data)[pixel_point + 1];
                    band->data.rgb.r[counter] = (*s_bmp_data)[pixel_point + 2];

                    counter++;
                }
            }
        }

        band->width = width;
        band->height = band_height;
        band->pos = i;
        band->bit_depth = s_bmp_info_header->bit_count;
        // printf("+++++[%s][%d][w:%d][h:%d][pos:%d][counter:%d]+++++\r\n", __FUNCTION__, __LINE__, width, band_height, i, counter);
    }

    pr_debug("+++++[%s][%d]+++++\r\n", __FUNCTION__, __LINE__);

    return 0;
}


void plane2bmp(image_t *image, uint8_t *bmp_data)
{
    band_plane_t *band = NULL, *node = NULL;
    int32_t real_width = 0;
    int32_t line_width = 0;

    uint32_t counter = 0;
    uint32_t cnt = 0;

    band = image->head_band;
    pr_debug("+++++[%s][%d][w:%d][h:%d]+++++\r\n", __FUNCTION__, __LINE__, band->width, band->height);

    /* memset((void *)bmp_data, 0, WIDTHBYTES(image->width * image->bit_depth) * image->height); */

    do
    {
        cnt++;

        if (band->is_gray)
        {
            real_width = (band->width / (8 / band->bit_depth));
            line_width = WIDTHBYTES(band->width * band->bit_depth);
            /* pr_debug("[%s] band cnt = %d\n", __func__, cnt); */
            /* pr_debug("[%s] real_width = %d\n", __func__, real_width); */
            /* pr_debug("[%s] line_width = %d\n", __func__, line_width); */
            /* pr_debug("[%s] height = %d\n", __func__, band->height); */
        }

        for(uint32_t i = 0; i < band->height; i++)
        {
            /* 灰度图处理 */
            if (band->is_gray)
            {
                /* pr_debug("[%s] bmp_data = %p\n", __func__, bmp_data); */
                /* pr_debug("[%s] band->data.gray = %p\n", __func__, band->data.gray); */

                uint8_t *bmp_data_point = bmp_data + (band->pos + i) * line_width;
                uint8_t *gray_point = band->data.gray + i * real_width;

                /* pr_debug("[%s] bmp_data_point = %p\n", __func__, bmp_data_point); */
                /* pr_debug("[%s] gray_point = %p\n", __func__, gray_point); */
                memcpy((void *)bmp_data_point, (void *)gray_point, real_width);
                /* pr_debug("[%s] [%d]\n", __func__, __LINE__); */
            }
            else
            {
                /* rgb图像处理 */
                /* pr_debug("[%s] [%d]\n", __func__, __LINE__); */
                for(uint32_t j = 0; j < band->width; j++)
                {
                    bmp_data[counter] = band->data.rgb.b[i * band->width + j];
                    bmp_data[counter + 1] = band->data.rgb.g[i * band->width + j];
                    bmp_data[counter + 2] = band->data.rgb.r[i * band->width + j];
                    counter += 3;
                }
                counter = ((counter + 3) / 4 )* 4;
                /* pr_debug("[%s] [%d]\n", __func__, __LINE__); */
            }
        }
        /* pr_debug("+++++[%s][%d][cnt:%d]+++++\r\n", __FUNCTION__, __LINE__,cnt); */
        if (band->next_band == NULL)
        {
            break;
        }

        node = band;
        /* printf("+++++[%s][%d][cnt:%d]+++++\r\n", __FUNCTION__, __LINE__,cnt); */
        band = band->next_band;
        /* printf("+++++[%s][%d][cnt:%d]+++++\r\n", __FUNCTION__, __LINE__,cnt); */

        /* free */
        /*
        if (band->is_gray)
        {
            free(node->data.gray);
            if (node->data.gray != NULL)
            {
                node->data.gray = NULL;
            }
        }
        else
        {
            if (node->data.rgb.b != NULL)
            {
                free(node->data.rgb.b);
                node->data.rgb.b = NULL;
            }
            if (node->data.rgb.g != NULL)
            {
                free(node->data.rgb.g);
                node->data.rgb.g = NULL;
            }
            if (node->data.rgb.r != NULL)
            {
                free(node->data.rgb.r);
                node->data.rgb.r = NULL;
            }
        }
    */
        /* pr_debug("[%s] [%d]\n", __func__, __LINE__); */
        //free(node);
        //node = NULL;
        /* pr_debug("[%s] [%d]\n", __func__, __LINE__); */
    
    }while(1);

    pr_debug("+++++[%s][%d][w:%d][h:%d][cnt:%d][counter:%d]+++++\r\n", __FUNCTION__, __LINE__, band->width, band->height, cnt, counter);
}

void insert_image(image_t *image, band_plane_t *band)
{
    if (band == NULL)
    {
        return;
    }

    if (image->head_band == NULL)
    {
        image->head_band = band;
        image->tail_band = band;
    }
    else
    {
        band->pre_band = image->tail_band;
        image->tail_band->next_band = band;
        image->tail_band = band;
    }
}

void free_band(band_plane_t *band)
{
    if (band == NULL)
        return;

    if (band->is_gray)
    {
        free(band->data.gray);
        band->data.gray = NULL;
    }
    else
    {
        free(band->data.rgb.b);
        band->data.rgb.b = NULL;
        free(band->data.rgb.g);
        band->data.rgb.g = NULL;
        free(band->data.rgb.r);
        band->data.rgb.r = NULL;
    }

    free(band);
}

band_plane_t *copy_band(band_plane_t *band)
{
    band_plane_t *dst_band;
    dst_band = (band_plane_t *)malloc(sizeof(band_plane_t));
    memset((void *)dst_band, 0, sizeof(band_plane_t));

    dst_band->width = band->width;
    dst_band->height = band->height;
    dst_band->pos = band->pos;
    dst_band->bit_depth = band->bit_depth;
    dst_band->is_gray = band->is_gray;
    dst_band->pre_band = band->pre_band;
    dst_band->next_band = band->next_band;

    pr_debug("[%s] [%d] dst_band->width = %d\n", __func__, __LINE__, dst_band->width);
    pr_debug("[%s] [%d] dst_band->height = %d\n", __func__, __LINE__, dst_band->height);

    int32_t band_size = dst_band->height * dst_band->width;
    pr_debug("[%s] [%d] band_size = %d\n", __func__, __LINE__, band_size);

    if (dst_band->is_gray)
    {
        dst_band->data.gray = (uint8_t*)malloc(band_size);
        memcpy(dst_band->data.gray, band->data.gray, band_size);
    }
    else
    {
        dst_band->data.rgb.r = (uint8_t*)malloc(band_size);
        dst_band->data.rgb.g = (uint8_t*)malloc(band_size);
        dst_band->data.rgb.b = (uint8_t*)malloc(band_size);
        memcpy(dst_band->data.rgb.r, band->data.rgb.r, band_size);
        memcpy(dst_band->data.rgb.g, band->data.rgb.g, band_size);
        memcpy(dst_band->data.rgb.b, band->data.rgb.b, band_size);
    }

    return dst_band;
}

