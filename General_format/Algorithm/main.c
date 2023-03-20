#include "../Utils/bmp.h"
#include "../Utils/Mat.h"
#include "../Utils/cv_utils.h"
#include <assert.h>
#include <stdlib.h>
#define FLOAT6(value) ((float)((int)(((value) + 0.0000005) * 1000000)) / 1000000)
#define DEBUG
#define MY_VERSION 1
/*running format:
* ./main 'input_path' 'output_path'
*/
uint8_t error_num = 0;
int main(int argc, char* argv[])
{

    //assert(argc == 3);
    if(argc != 3){
        printf("Missing parameter!\nPlease run in this format: ./main 'input_path' 'output_path'\n");
        return 0;
    }
    /*Initialization:
    * Input&Output: bmp
    * temp_in: image
    * temp_out: new_image
    */
    Mat_t mat, dst, tmp;

    BMP bmp, out_bmp;
    int loop_num =1;
    //printf("loop_num=%d\n",loop_num);
    /*read BMP*/
    int32_t err = read_bmp(argv[1], &bmp);

    assert(err == 0);

    /*transformation*/
    bmp2Mat(&bmp, &mat);

    copy_MatHead(&mat, &dst);
    copyMakeBorder(&mat, &tmp, 1);
    int size = mat.width * mat.height * mat.cn;
    if(mat.cn == 1){
        dst.data.gray = (uint8_t*)malloc(size*sizeof(uint8_t));
        memset(dst.data.gray, 0, size);
    }else{
        dst.data.rgb = (rgb_t*)malloc(size*sizeof(uint8_t));
        memset(dst.data.rgb, 0, size);
    }
    if(MY_VERSION){
        bilateralFilter_simd_beta(&tmp, &dst, 2, 35, 15);
    }
    else{
        double sigma_s = 15;
        double sigma_r = 35;
        image_t image;
        band_plane_t * band;

        init_bilateral_table(sigma_s, sigma_r);
        bmp2plane(&bmp, &image);
        band = image.head_band;
        do{

            bilateralFilter(band, 1, loop_num);
            if(band->next_band == NULL)
                break;
            band = band->next_band;
        }while(1);
        Plane2Mat(&image, &dst);
    }

    
    /*retransformation*/
    Mat2NewBmp(&dst, &bmp, &out_bmp);

    /*free*/
    Mat_free(&mat);
    Mat_free(&tmp);
    
    /*save*/
    pr_debug("[%s][%d]\n", __func__, __LINE__);
    save_bmp(argv[2], &out_bmp);
    pr_debug("[%s][%d]\n", __func__, __LINE__);
    
    return 0;
}

