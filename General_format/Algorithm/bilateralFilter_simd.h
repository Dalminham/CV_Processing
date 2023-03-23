#ifndef __BILATERALFILTER_H
#define __BILATERALFILTER_H
#include "../Utils/Mat.h"
#include"../Utils/utility.h"

#define CV_SIMD 0

void bilateralFilterInvoker_float(Mat_t *_dst, Mat_t* _temp, int _radius, int _maxk,
        int* _space_ofs, float *_space_weight, float *_color_weight)
{
    pr_debug("[%s]bilateralInvoker Head\n",__func__);
    int i, j, cn = _dst->cn, k;
    uint32_t width, height, bwidth;
    width = _dst->width;
    height = _dst->height;
    bwidth = _temp->width;
    if(cn == 1)
    {
        for(i=0; i < height; i++)
        {
            const uint8_t* sptr;

            sptr = _temp->data.gray + (i + _radius)*bwidth + _radius;
            uint8_t* dptr = _dst->data.gray + i * width;

            uint32_t buf_size = alignSize(width, CV_SIMD_WIDTH) + width + CV_SIMD_WIDTH - 1;
            float * buf = (float*)malloc(buf_size*sizeof(float));
            memset(buf, 0, buf_size*sizeof(float));
            float *sum = alignPtr_f(buf, CV_SIMD_WIDTH);
            float *wsum = sum + alignSize(width, CV_SIMD_WIDTH);
            k=0;
            for(; k<= _maxk-4; k+=4)
            {
                const uint8_t* ksptr0 = sptr + _space_ofs[k];
                const uint8_t* ksptr1 = sptr + _space_ofs[k+1];
                const uint8_t* ksptr2 = sptr + _space_ofs[k+2];
                const uint8_t* ksptr3 = sptr + _space_ofs[k+3];
                j=0;

                for(; j < width; j++)
                {

                    int rval = sptr[j];

                    int val = ksptr0[j];
                    float w = _space_weight[k] * _color_weight[abs(val-rval)];
                    wsum[j] += w;
                    sum[j] += val * w;

                    val = ksptr1[j];
                    w = _space_weight[k+1] * _color_weight[abs(val - rval)];
                    wsum[j] += w;
                    sum[j] += val * w;

                    val = ksptr2[j];
                    w = _space_weight[k+2] * _color_weight[abs(val-rval)];
                    wsum[j] += w;
                    sum[j] += val * w;

                    val = ksptr3[j];
                    w = _space_weight[k+3] * _color_weight[abs(val-rval)];
                    wsum[j] += w;
                    sum[j] += val * w;
                }
            }

            for(; k < _maxk; k++)
            {
                const uint8_t* ksptr = sptr + _space_ofs[k];
                j = 0;

                for(; j<width; j++)
                {
                    int val = ksptr[j];
                    float w = _space_weight[k] * _color_weight[abs(val-sptr[j])];
                    wsum[j] += w;
                    sum[j] += val * w;
                }
            }
            j = 0;

            for(; j < width;j++)
            {
                assert(wsum[j] != 0);
                dptr[j] = ((uint8_t)(sum[j]/wsum[j]));
            }
            free(buf);
        }
    }else
    {
        assert(cn == 3);
        for(i=0;i<height;i++){

            const uint8_t* sptr;

            sptr = (uint8_t*)_temp->data.rgb + (i + _radius)*bwidth*cn + _radius*cn;
            uint8_t* dptr = (uint8_t*)_dst->data.rgb + i * width*cn;

            uint32_t buf_size = alignSize(width, CV_SIMD_WIDTH)*3 + width + CV_SIMD_WIDTH - 1;
            float* buf = (float *)malloc(buf_size * sizeof(float));
            memset(buf, 0, sizeof(float)*buf_size);
            float *sum_r = alignPtr_f(buf, CV_SIMD_WIDTH);
            float *sum_g = sum_r + alignSize(width, CV_SIMD_WIDTH);
            float *sum_b = sum_g + alignSize(width, CV_SIMD_WIDTH);
            float *wsum = sum_b + alignSize(width, CV_SIMD_WIDTH);
            k=0;
            for(; k <= _maxk-4; k+=4)
            {
                const uint8_t* ksptr0 = sptr + _space_ofs[k];
                const uint8_t* ksptr1 = sptr + _space_ofs[k+1];
                const uint8_t* ksptr2 = sptr + _space_ofs[k+2];
                const uint8_t* ksptr3 = sptr + _space_ofs[k+3];
                const uint8_t* rsptr = sptr;
                j=0;


                for(; j < width; j++, rsptr += 3, ksptr0 += 3, ksptr1 += 3, ksptr2 += 3, ksptr3 += 3)
                {

                    int rr = rsptr[0], rg = rsptr[1], rb = rsptr[2];

                    int r = ksptr0[0], g = ksptr0[1], b = ksptr0[2];
                    float w = _space_weight[k] * _color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)];
                    wsum[j] += w;
                    sum_b[j] += b * w;
                    sum_g[j] += g * w;
                    sum_r[j] += r * w;

                    r = ksptr1[0], g = ksptr1[1], b = ksptr1[2];
                    w = _space_weight[k+1] * _color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)];
                    wsum[j] += w;
                    sum_b[j] += b * w;
                    sum_g[j] += g * w;
                    sum_r[j] += r * w;

                    r = ksptr2[0], g = ksptr2[1], b = ksptr2[2];
                    w = _space_weight[k+2] * _color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)];
                    wsum[j] += w;
                    sum_b[j] += b * w;
                    sum_g[j] += g * w;
                    sum_r[j] += r * w;

                    r = ksptr3[0], g = ksptr3[1], b = ksptr3[2];
                    w = _space_weight[k+3] * _color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)];
                    wsum[j] += w;
                    sum_b[j] += b * w;
                    sum_g[j] += g * w;
                    sum_r[j] += r * w;

                }
            }

            for(; k< _maxk; k++)
            {
                const uint8_t * ksptr = sptr + _space_ofs[k];
                const uint8_t * rsptr = sptr;
                j = 0;

                for(; j < width; j++, ksptr += 3, rsptr += 3)
                {
                    int r = ksptr[0], g = ksptr[1], b = ksptr[2];
                    float w = _space_weight[k] * _color_weight[abs(r - rsptr[0]) + abs(g - rsptr[1]) + abs(b - rsptr[2])];
                    wsum[j] += w;
                    sum_r[j] += r*w;
                    sum_g[j] += g*w;
                    sum_b[j] += b*w;
                }

            }
            j = 0;
            for(; j < width; j++)
            {
                //printf("[%d, %d]wrgb={%f, %f, %f, %f}\n",i,j, wsum[j], sum_r[j], sum_g[j], sum_b[j]);
                assert(wsum[j] > 0);
                wsum[j] = 1.0f/wsum[j];
                *(dptr++) = (uint8_t)(sum_r[j]*wsum[j]);
                *(dptr++) = (uint8_t)(sum_g[j]*wsum[j]);
                *(dptr++) = (uint8_t)(sum_b[j]*wsum[j]);
            }
            free(buf);
        }

    }

}


void bilateralFilterInvoker_fixed(Mat_t *_dst, Mat_t* _temp, int _radius, int _maxk,
        int* _space_ofs, uint32_t *_space_weight, uint32_t *_color_weight)
{
    int i, j, cn = _dst->cn, k;
    uint32_t width, height, bwidth;
    width = _dst->width;
    height = _dst->height;
    bwidth = _temp->width;
    if(cn == 1)
    {
        for(i=0; i < height; i++)
        {
            const uint8_t* sptr;

            sptr = _temp->data.gray + (i + _radius)*bwidth + _radius;
            uint8_t* dptr = _dst->data.gray + i * width;

            uint32_t buf_size = alignSize(width, CV_SIMD_WIDTH) + width + CV_SIMD_WIDTH - 1;
            uint32_t * buf = (uint32_t*)malloc(buf_size*sizeof(uint32_t));
            memset(buf, 0, buf_size*sizeof(uint32_t));
            uint32_t *sum = alignPtr_w(buf, CV_SIMD_WIDTH);
            uint32_t *wsum = sum + alignSize(width, CV_SIMD_WIDTH);
            k=0;
            for(; k<= _maxk-4; k+=4)
            {
                const uint8_t* ksptr0 = sptr + _space_ofs[k];
                const uint8_t* ksptr1 = sptr + _space_ofs[k+1];
                const uint8_t* ksptr2 = sptr + _space_ofs[k+2];
                const uint8_t* ksptr3 = sptr + _space_ofs[k+3];
                j=0;

                for(; j < width; j++)
                {

                    int rval = sptr[j];

                    int val = ksptr0[j];
                    uint32_t w = _space_weight[k] * _color_weight[abs(val-rval)] >> 18;
                    wsum[j] += w;
                    sum[j] += val * w;

                    val = ksptr1[j];
                    w = _space_weight[k+1] * _color_weight[abs(val - rval)] >> 18;
                    wsum[j] += w;
                    sum[j] += val * w;

                    val = ksptr2[j];
                    w = _space_weight[k+2] * _color_weight[abs(val-rval)] >> 18;
                    wsum[j] += w;
                    sum[j] += val * w;

                    val = ksptr3[j];
                    w = _space_weight[k+3] * _color_weight[abs(val-rval)] >> 18;
                    wsum[j] += w;
                    sum[j] += val * w;
                }
            }
            for(; k < _maxk; k++)
            {
                const uint8_t* ksptr = sptr + _space_ofs[k];
                j = 0;
                for(; j<width; j++)
                {
                    uint32_t val = ksptr[j];
                    uint32_t w = (_space_weight[k] * _color_weight[abs(val-sptr[j])])>> 18;
                    wsum[j] += w;
                    sum[j] += val * w;
                }
            }
            j = 0;
            for(; j < width;j++)
            {
                assert(wsum[j] != 0);
                dptr[j] = ((uint8_t)(sum[j]/wsum[j]));
            }
            free(buf);
        }
    }
    else
    {
        assert(cn == 3);
        for(i=0;i<height;i++){

            const uint8_t* sptr;

            sptr = (uint8_t*)_temp->data.rgb + (i + _radius)*bwidth*cn + _radius*cn;
            uint8_t* dptr = (uint8_t*)_dst->data.rgb + i * width*cn;

            uint32_t buf_size = alignSize(width, CV_SIMD_WIDTH)*3 + width + CV_SIMD_WIDTH - 1;
            uint32_t* buf = (uint32_t *)malloc(buf_size * sizeof(uint32_t));
            memset(buf, 0, sizeof(uint32_t)*buf_size);
            uint32_t *sum_r = alignPtr_w(buf, CV_SIMD_WIDTH);
            uint32_t *sum_g = sum_r + alignSize(width, CV_SIMD_WIDTH);
            uint32_t *sum_b = sum_g + alignSize(width, CV_SIMD_WIDTH);
            uint32_t *wsum = sum_b + alignSize(width, CV_SIMD_WIDTH);
            k=0;
            for(; k <= _maxk-4; k+=4)
            {
                const uint8_t* ksptr0 = sptr + _space_ofs[k];
                const uint8_t* ksptr1 = sptr + _space_ofs[k+1];
                const uint8_t* ksptr2 = sptr + _space_ofs[k+2];
                const uint8_t* ksptr3 = sptr + _space_ofs[k+3];
                const uint8_t* rsptr = sptr;
                j=0;

                for(; j < width; j++, rsptr += 3, ksptr0 += 3, ksptr1 += 3, ksptr2 += 3, ksptr3 += 3)
                {
                    int rr = rsptr[0], rg = rsptr[1], rb = rsptr[2];

                    int r = ksptr0[0], g = ksptr0[1], b = ksptr0[2];
                    uint32_t w = (_space_weight[k] * _color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)]) >> 18;
                    wsum[j] += w;
                    sum_b[j] += b * w;
                    sum_g[j] += g * w;
                    sum_r[j] += r * w;

                    r = ksptr1[0], g = ksptr1[1], b = ksptr1[2];
                    w = (_space_weight[k+1] * _color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)]) >> 18;
                    wsum[j] += w;
                    sum_b[j] += b * w;
                    sum_g[j] += g * w;
                    sum_r[j] += r * w;

                    r = ksptr2[0], g = ksptr2[1], b = ksptr2[2];
                    w = _space_weight[k+2] * _color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)] >> 18;
                    wsum[j] += w;
                    sum_b[j] += b * w;
                    sum_g[j] += g * w;
                    sum_r[j] += r * w;

                    r = ksptr3[0], g = ksptr3[1], b = ksptr3[2];
                    w = _space_weight[k+3] * _color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)] >> 18;
                    wsum[j] += w;
                    sum_b[j] += b * w;
                    sum_g[j] += g * w;
                    sum_r[j] += r * w;
                }
            }

            for(; k< _maxk; k++)
            {
                const uint8_t * ksptr = sptr + _space_ofs[k];
                const uint8_t * rsptr = sptr;
                j = 0;
                for(; j < width; j++, ksptr += 3, rsptr += 3)
                {
                    int r = ksptr[0], g = ksptr[1], b = ksptr[2];
                    uint32_t w = _space_weight[k] * _color_weight[abs(r - rsptr[0]) + abs(g - rsptr[1]) + abs(b - rsptr[2])] >> 18;
                    wsum[j] += w;
                    sum_r[j] += r*w;
                    sum_g[j] += g*w;
                    sum_b[j] += b*w;
                }

            }
            j = 0;
            for(; j < width; j++)
            {
                assert(wsum[j] > 0);
                *(dptr++) = (uint8_t)(sum_r[j]/wsum[j]);
                *(dptr++) = (uint8_t)(sum_g[j]/wsum[j]);
                *(dptr++) = (uint8_t)(sum_b[j]/wsum[j]);
            }
            free(buf);
        }

    }

}


void bilateralFilterInvoker_fraction(Mat_t *_dst, Mat_t* _temp, int _radius, int _maxk,
        int* _space_ofs, fraction *_space_weight, fraction *_color_weight)
{
    int i, j, cn = _dst->cn, k;
    uint32_t width, height, bwidth;
    width = _dst->width;
    height = _dst->height;
    bwidth = _temp->width;
    if(cn == 1)
    {
        for(i=0; i < height; i++)
        {
            const uint8_t* sptr;

            sptr = _temp->data.gray + (i + _radius)*bwidth + _radius;
            uint8_t* dptr = _dst->data.gray + i * width;

            uint32_t buf_size = alignSize(width, CV_SIMD_WIDTH) + width + CV_SIMD_WIDTH - 1;
            fraction * buf = (fraction*)malloc(buf_size*sizeof(fraction));
            memset(buf, 0, buf_size*sizeof(fraction));
            fraction *sum = alignPtr_fr(buf, CV_SIMD_WIDTH);
            fraction *wsum = sum + alignSize(width, CV_SIMD_WIDTH);
            k=0;
            for(; k<= _maxk-4; k+=4)
            {
                const uint8_t* ksptr0 = sptr + _space_ofs[k];
                const uint8_t* ksptr1 = sptr + _space_ofs[k+1];
                const uint8_t* ksptr2 = sptr + _space_ofs[k+2];
                const uint8_t* ksptr3 = sptr + _space_ofs[k+3];
                j=0;

                for(; j < width; j++)
                {

                    int rval = sptr[j];
                    int val = ksptr0[j];

                    fraction w,fr_val;

                    i2fr(val, &fr_val);
                    fr_mul(&_space_weight[k], &_color_weight[abs(val-rval)], &w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum[j], &fr_val, &w);

                    val = ksptr1[j];
                    i2fr(val, &fr_val);
                    fr_mul(&_space_weight[k+1], &_color_weight[abs(val-rval)], &w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum[j], &fr_val, &w);

                    val = ksptr2[j];
                    i2fr(val, &fr_val);
                    fr_mul(&_space_weight[k+1], &_color_weight[abs(val-rval)], &w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum[j], &fr_val, &w);

                    val = ksptr3[j];
                    i2fr(val, &fr_val);
                    fr_mul(&_space_weight[k+1], &_color_weight[abs(val-rval)], &w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum[j], &fr_val, &w);
                }
            }
            for(; k < _maxk; k++)
            {
                const uint8_t* ksptr = sptr + _space_ofs[k];
                j = 0;
                for(; j<width; j++)
                {
                    uint32_t val = ksptr[j];
                    fraction fr_val, w;
                    i2fr(val, &fr_val);
                    fr_mul(&_space_weight[k], &_color_weight[abs(val-sptr[j])], &w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum[j], &fr_val, &w);
                }
            }
            j = 0;
            for(; j < width;j++)
            {
                assert(wsum[j].data != 0);
                dptr[j] = ((uint8_t)(sum[j].data/wsum[j].data));
            }
            free(buf);
        }
    }
    else
    {
        assert(cn == 3);
        for(i=0;i<height;i++){

            const uint8_t* sptr;

            sptr = (uint8_t*)_temp->data.rgb + (i + _radius)*bwidth*cn + _radius*cn;
            uint8_t* dptr = (uint8_t*)_dst->data.rgb + i * width*cn;

            uint32_t buf_size = alignSize(width, CV_SIMD_WIDTH)*3 + width + CV_SIMD_WIDTH - 1;
            fraction* buf = (fraction*)malloc(buf_size * sizeof(fraction));
            memset(buf, 0, sizeof(fraction)*buf_size);
            fraction *sum_r = alignPtr_fr(buf, CV_SIMD_WIDTH);
            fraction *sum_g = sum_r + alignSize(width, CV_SIMD_WIDTH);
            fraction *sum_b = sum_g + alignSize(width, CV_SIMD_WIDTH);
            fraction *wsum = sum_b + alignSize(width, CV_SIMD_WIDTH);
            k=0;
            for(; k <= _maxk-4; k+=4)
            {
                const uint8_t* ksptr0 = sptr + _space_ofs[k];
                const uint8_t* ksptr1 = sptr + _space_ofs[k+1];
                const uint8_t* ksptr2 = sptr + _space_ofs[k+2];
                const uint8_t* ksptr3 = sptr + _space_ofs[k+3];
                const uint8_t* rsptr = sptr;
                j=0;

                for(; j < width; j++, rsptr += 3, ksptr0 += 3, ksptr1 += 3, ksptr2 += 3, ksptr3 += 3)
                {
                    int rr = rsptr[0], rg = rsptr[1], rb = rsptr[2];

                    int r = ksptr0[0], g = ksptr0[1], b = ksptr0[2];
                    fraction fr,fg,fb;
                    i2fr(r,&fr);
                    i2fr(g,&fg);
                    i2fr(b,&fb);
                    fraction w;
                    fr_mul(&_space_weight[k], &_color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)],&w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum_b[j],&fb, &w);
                    fr_mac(&sum_g[j],&fg, &w);
                    fr_mac(&sum_r[j],&fr, &w);

                    r = ksptr1[0], g = ksptr1[1], b = ksptr1[2];
                    i2fr(r,&fr);
                    i2fr(g,&fg);
                    i2fr(b,&fb);
                    fr_mul(&_space_weight[k], &_color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)],&w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum_b[j],&fb, &w);
                    fr_mac(&sum_g[j],&fg, &w);
                    fr_mac(&sum_r[j],&fr, &w);

                    r = ksptr2[0], g = ksptr2[1], b = ksptr2[2];
                    i2fr(r,&fr);
                    i2fr(g,&fg);
                    i2fr(b,&fb);
                    fr_mul(&_space_weight[k], &_color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)],&w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum_b[j],&fb, &w);
                    fr_mac(&sum_g[j],&fg, &w);
                    fr_mac(&sum_r[j],&fr, &w);

                    r = ksptr3[0], g = ksptr3[1], b = ksptr3[2];
                    i2fr(r,&fr);
                    i2fr(g,&fg);
                    i2fr(b,&fb);
                    fr_mul(&_space_weight[k], &_color_weight[abs(r - rr) + abs(g-rg) + abs(b - rb)],&w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum_b[j],&fb, &w);
                    fr_mac(&sum_g[j],&fg, &w);
                    fr_mac(&sum_r[j],&fr, &w);
                }
            }

            for(; k< _maxk; k++)
            {
                const uint8_t * ksptr = sptr + _space_ofs[k];
                const uint8_t * rsptr = sptr;
                j = 0;
                for(; j < width; j++, ksptr += 3, rsptr += 3)
                {
                    int r = ksptr[0], g = ksptr[1], b = ksptr[2];
                    fraction fr,fg,fb,w;
                    i2fr(r,&fr);
                    i2fr(g,&fg);
                    i2fr(b,&fb);
                    fr_mul( &_space_weight[k], &_color_weight[abs(r - rsptr[0]) + abs(g - rsptr[1]) + abs(b - rsptr[2])], &w);
                    fr_acc(&wsum[j], &w);
                    fr_mac(&sum_b[j],&fb, &w);
                    fr_mac(&sum_g[j],&fg, &w);
                    fr_mac(&sum_r[j],&fr, &w);
                }

            }
            j = 0;
            for(; j < width; j++)
            {
                assert(wsum[j].data > 0);
                *(dptr++) = (uint8_t)(sum_r[j].data/wsum[j].data);
                *(dptr++) = (uint8_t)(sum_g[j].data/wsum[j].data);
                *(dptr++) = (uint8_t)(sum_b[j].data/wsum[j].data);
            }
            free(buf);
        }

    }

}
#endif
