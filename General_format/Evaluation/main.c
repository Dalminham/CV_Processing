#include <stdio.h>
#include "../Utils/bmp.h"
#include "../Utils/Mat.h"
#include "../Utils/cv_utils.h"
#include "../Utils/Array_IO.h"
#include <assert.h>
#include <stdlib.h>
#define FLOAT6(value) ((float)((int)(((value) + 0.0000005) * 1000000)) / 1000000)
#define DEBUG
#define MY_VERSION 1

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

    /*Array IO*/
    
    uint32_t height = mat.height, width = mat.width, cn = mat.cn;
    uint8_t *array = (uint8_t*)malloc(height*width*cn*sizeof(uint8_t));
    memset(array,0, height*width*cn);
    //Mat2Array(&mat,array);
    //Array_Print(array,height*width*cn);
    //Mat2Array(&mat,array);
    //Array_Save("./Array/array.bin",array,height*width*cn*sizeof(uint8_t));
    //Array_Print(array, height*width*cn);
    //memset(array,0, height*width*cn);
    Array_Read("./Array/array.bin",array,height*width*cn*sizeof(uint8_t));
    Array2Mat(&mat,array);

    /*retransformation*/
    Mat2NewBmp(&mat, &bmp, &out_bmp);

    /*free*/
    Mat_free(&mat);
    Mat_free(&tmp);
    
    /*save*/
    pr_debug("[%s][%d]\n", __func__, __LINE__);
    save_bmp(argv[2], &out_bmp);
    pr_debug("[%s][%d]\n", __func__, __LINE__);
    
    return 0;
}