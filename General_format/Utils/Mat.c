#include "Mat.h"

void bmp2Mat(BMP *bmp, Mat_t* mat)
{
     uint8_t *s_bmp_data = bmp->bmp_data;
     /* bmp_file_header_t *s_bmp_file_header = &(bmp->bmp_file_header); */
     bmp_info_header_t *s_bmp_info_header = &(bmp->bmp_info_header);

     /* 获取原位图数据高宽，并计算数据实际的宽度（4字节对齐） */
     int32_t width = s_bmp_info_header->width;
     int32_t height = s_bmp_info_header->height;
     int32_t line_width = WIDTHBYTES(width * s_bmp_info_header->bit_count);

     /* 计算每个像素点的字节数 */
     uint8_t cn = s_bmp_info_header->bit_count / 8;
     uint32_t real_width = width * cn;
     uint8_t bit_count = s_bmp_info_header->bit_count;



    mat->width = s_bmp_info_header->width;
    mat->height = s_bmp_info_header->height;
    mat->cn = cn;

    if(cn == 1)
    {
      mat->data.gray = (uint8_t*)malloc(height*width*sizeof(uint8_t));
      memset(mat->data.gray, 0, height*width*sizeof(uint8_t));
    }
    else{
         mat->data.rgb = (rgb_t*)malloc(height*width*sizeof(rgb_t));
         memset(mat->data.rgb, 0, height*width*sizeof(rgb_t));
    }

    uint32_t counter = 0;
    for(uint32_t i=0; i<height;i++)
    {
        uint8_t *bmp_data_point = s_bmp_data + i * line_width * mat->cn;
        if (cn == 1)
        {
           uint8_t *gray_point = mat->data.gray + i * real_width;
           memcpy((void *)gray_point, (void *)bmp_data_point, real_width);
        }
        else{
            for(uint32_t j=0; j<width; j++)
            {
                uint32_t pixel_point = i * line_width + j * 3;
                mat->data.rgb[counter].b = s_bmp_data[pixel_point+0];
                mat->data.rgb[counter].g = s_bmp_data[pixel_point+1];
                mat->data.rgb[counter].r = s_bmp_data[pixel_point+2];
                counter++;
            }
        }
    }
}

/*Replace the bmp_data with the mat, no other changes*/
void Mat2bmp(Mat_t * mat, uint8_t *bmp_data)
{
    uint32_t width, height;
    uint8_t cn;
    width = mat->width;
    height = mat->height;
    cn = mat->cn;
    uint32_t bit_depth = cn == 1?8:24, counter = 0, pix=0, real_width = width * cn;
    uint32_t line_width = WIDTHBYTES(width * bit_depth);
    for(uint32_t i=0; i<height; i++)
    {
        uint8_t *bmp_data_point = bmp_data + i * line_width;
        if(cn==1){
            uint8_t *gray_point = mat->data.gray + i * real_width;
            memcpy((void *)bmp_data_point, (void *)gray_point, real_width);
        }else{
        uint8_t *bmp_data_point = bmp_data + i * line_width;
            for(uint32_t j=0, counter = 0; j<width; j++)
            {
                bmp_data_point[counter] = mat->data.rgb[pix].b;
                bmp_data_point[counter+1] = mat->data.rgb[pix].g;
                bmp_data_point[counter+2] = mat->data.rgb[pix].r;
                pix++;
                counter+=3;
            }
        }
    }
}


void copy_file_header(bmp_file_header_t* src, bmp_file_header_t* dst)
{
    dst->type = src->type;
    dst->size = src->size;
    dst->reserved1 = src->reserved1;
    dst->reserved2 = src->reserved2;
    dst->off_bits = src->off_bits;
}

void copy_info_header(bmp_info_header_t* src, bmp_info_header_t* dst)
{
    dst->size = src->size;
    dst->width = src->width;
    dst->height = src->height;
    dst->planes = src->planes;
    dst->bit_count = src->bit_count;
    dst->compression = src->compression;
    dst->size_image = src->size_image;
    dst->x_pels_permeter = src->x_pels_permeter;
    dst->y_pels_permeter = src->y_pels_permeter;
    dst->clr_used = src->clr_used;
    dst->clr_important = src->clr_important;
}

void copy_palette(uint8_t * src, uint8_t * dst)
{
    for(int i=0; i<1024; i++)
    {
        dst[i] = src[i];
    }
}
/*Create a new Bmp*/
void Mat2NewBmp(Mat_t * mat, BMP* src, BMP*dst)
{
    copy_file_header(&src->bmp_file_header, &dst->bmp_file_header);
    copy_info_header(&src->bmp_info_header, &dst->bmp_info_header);
    copy_palette(src->palette, dst->palette);
    uint8_t *bmp_data_point;
    uint8_t bit_depth = mat->cn == 1?8:24;
    uint32_t line_width = WIDTHBYTES(mat->width * bit_depth);
    uint32_t width, height, cn, img_size;
    width = mat->width;
    height = mat->height;
    cn = mat->cn;
    img_size = line_width * height;

    dst->bmp_info_header.width = width;
    dst->bmp_info_header.height = height;

    dst->bmp_file_header.size = img_size + dst->bmp_file_header.off_bits;
    dst->bmp_info_header.size = sizeof(bmp_info_header_t);
    dst->bmp_info_header.size_image = img_size;
    //

    uint8_t * data_p = (uint8_t*)malloc(img_size * sizeof(uint8_t));
    memset(data_p, 0, img_size * sizeof(uint8_t));
    uint32_t pix = 0, count = 0;
    if(cn == 1){
        for(uint32_t i=0; i < height; i++)
        {
            bmp_data_point = data_p + i * line_width;
            for(uint32_t j=0; j < width; j++)
            {
                bmp_data_point[j] = mat->data.gray[pix];
                pix++;
            }
        }
    }else
    {
        for(uint32_t i=0; i < height; i++)
        {
            bmp_data_point = data_p + i * line_width;
            for(uint32_t j=0, count=0; j < width; j++)
            {
                bmp_data_point[count] = mat->data.rgb[pix].b;
                bmp_data_point[count+1] = mat->data.rgb[pix].g;
                bmp_data_point[count+2] = mat->data.rgb[pix].r;
                count += 3;
                pix++;
            }
        }
    }
    dst->bmp_data = data_p;
}

void Mat_free(Mat_t * mat)
{
    if(mat->cn == 1)
    {
        free(mat->data.gray);
    }else{
        free(mat->data.rgb);
    }
}

void copy_MatHead(Mat_t *src, Mat_t* dst)
{
    dst->width = src->width;
    dst->height = src->height;
    dst->cn = src->cn;
}



//For evaluation
void Mat2Plane(Mat_t *mat, image_t *image){
    uint32_t height = mat->height, width = mat->width, cn = mat->cn;
    image->width = width;
    image->height = height;
    image->bit_depth = cn * 8;
    image->is_gray = cn == 1;
    image->head_band = NULL;
    image->tail_band = NULL;
    uint32_t band_height = 32;
    uint8_t ** s_mat_data = &(mat->data.gray);
    for(uint32_t i=0; i < height; i += 32)
    {
        uint32_t counter = 0;
        band_plane_t * band = (band_plane_t*)malloc(sizeof(band_plane_t));
        memset((void*)band, 0, sizeof(band_plane_t));

        insert_image(image, band);

        if( cn == 1)
        {

            band->is_gray = 1;
        }
        else
        {

            band->is_gray = 0;
        }
        band->data.gray = (uint8_t*)malloc(32 * width);
        band->data.rgb.r = (uint8_t*)malloc(32 * width);
        band->data.rgb.g = (uint8_t*)malloc(32 * width);
        band->data.rgb.b = (uint8_t*)malloc(32 * width);
        memset(band->data.gray, 0, 32*width);
        memset(band->data.rgb.r, 0, 32*width);
        memset(band->data.rgb.b, 0, 32*width);
        memset(band->data.rgb.g, 0, 32*width);
        if ((height - i) < 32)
        {
            band_height = height - i;
        }
        else
        {
            band_height = 32;
        }

        for(uint32_t k = 0; k < band_height; k++)
        {
            if(cn == 1)
            {
                uint8_t * mat_data_point = *s_mat_data + (i+k) *width;
                uint8_t * gray_point = band->data.gray + k * width;
                memcpy((void*)gray_point, (void*)mat_data_point, width);
            }
            else
            {
                for (int j=0; j < width; j++)
                {
                    band->data.rgb.b[counter] = mat->data.rgb[i*width + counter].b;
                    band->data.rgb.g[counter] = mat->data.rgb[i*width + counter].g;
                    band->data.rgb.r[counter] = mat->data.rgb[i*width + counter].r;

                    counter++;
                }
            }
        }
        band->width = width;
        band->height = band_height;
        band->pos = i;
        band->bit_depth = 8 * cn;
        band->image_width = image->width;
        band->image_height = image->height;
    }
    return;
}

void Plane2Mat(image_t *image, Mat_t *mat)
{
    uint32_t width = image->width, height = image->height, cn;
    cn = (image->is_gray == 1)? 1 : 3;
    mat->width = width;
    mat->height = height;
    mat->cn = cn;
    uint8_t **s_mat_data = &(mat->data.gray);

    if(cn == 1)
    {
        mat->data.gray = (uint8_t*)malloc(sizeof(uint8_t)*width * height);
        memset(mat->data.gray, 0, sizeof(uint8_t)*width * height);
    }
    else
    {
        mat->data.rgb = (rgb_t*)malloc(sizeof(rgb_t)*width*height);
        memset(mat->data.rgb, 0, sizeof(rgb_t)*width*height);
    }

    uint32_t counter = 0, cnt = 0;
    band_plane_t * band = image->head_band;
    //pr_debug("[%s][%d]\n", __func__, __LINE__);
    do
    {
        cnt++;

        for(uint32_t i=0; i < band->height; i++)
        {
            if(cn == 1)
            {
                uint8_t * mat_data_point = *s_mat_data + (band->pos + i) * width;
                uint8_t * gray_point = band->data.gray + i * width;

                memcpy((void *)mat_data_point, (void *)gray_point, width);
            }
            else
            {
                for(uint32_t j =0; j < band->width; j++)
                {
                    //pr_debug("[%s][%d]\n", __func__, __LINE__);

                    mat->data.rgb[counter].r = band->data.rgb.r[i * band->width + j];
                    mat->data.rgb[counter].g = band->data.rgb.g[i * band->width + j];
                    mat->data.rgb[counter].b = band->data.rgb.b[i * band->width + j];
                    counter++;


                }
            }
        }

        if(band->next_band == NULL)
        {
            break;
        }


        band = band->next_band;
    }while(1);
}