#include "../Utils/bmp.h"
#include "../Utils/Mat.h"
#include "../Utils/cv_utils.h"
#include "../Utils/Array_IO.h"
#include "../Utils/String_Tool.h"
#include <assert.h>
#include <stdlib.h>
#define FLOAT6(value) ((float)((int)(((value) + 0.0000005) * 1000000)) / 1000000)
#define DEBUG

/*running format:
* ./main 'input_path' 'output_path'
*/
uint8_t error_num = 0;

int Process_Array(char* array_path, int row_width, int cn, char* write_path){
    Mat_t mat, dst, tmp;
    mat.height=32;
    mat.width = row_width;
    mat.cn = cn;
    int size = mat.width * mat.height * mat.cn;
    uint8_t* Data_array = (uint8_t*)malloc(size*sizeof(uint8_t));
    memset(Data_array, 0, size);

    if(cn == 1){
        mat.data.gray = (uint8_t*)malloc(size*sizeof(uint8_t));
        memset(mat.data.gray, 0 ,size);
    }else{
        mat.data.rgb = (rgb_t*)malloc(size*sizeof(uint8_t));
        memset(mat.data.rgb, 0, size);
    }

    /*Array IO*/
    Array_Read(array_path, Data_array, size);
    Array2Mat(&mat,Data_array);

    copy_MatHead(&mat, &dst);
    copyMakeBorder(&mat, &tmp, 1);
    /*free mat*/
    Mat_free(&mat);

    if(cn == 1){
        dst.data.gray = (uint8_t*)malloc(size*sizeof(uint8_t));
        memset(dst.data.gray, 0, size);
    }else{
        dst.data.rgb = (rgb_t*)malloc(size*sizeof(uint8_t));
        memset(dst.data.rgb, 0, size);
    }

    bilateralFilter_simd_beta(&tmp, &dst, 2, 35, 15);
    //bilateralFilter_fraction(&tmp,&dst, 2, 35, 15);

    Mat2Array(&dst,Data_array);

    //Array Write
    /*
    char write_name[] = "../Evaluation/Array/output_array";
    char* write_path;
    num_name_str(write_name, num, write_path);
    */
    Array_Save(write_path,Data_array,size*sizeof(uint8_t));


    /*free*/
    free(Data_array);
    Mat_free(&tmp);
    Mat_free(&dst);
}

int main(int argc, char* argv[])
{
    if(argc < 2){
        printf("Input format Error!\n");
        return 0;
    }
    int opt = atoi(argv[1]);
    if(opt == 1){
        assert(argc == 5);
        char* read_path  = argv[2];
        int row_width    = atoi(argv[3]);
        char* write_path = argv[4];

        Process_Array(read_path, row_width, 3, write_path);
    }else if(opt == 0){
        //TODO: bmp2Array
        BMP bmp;
        int32_t err = read_bmp(argv[2], &bmp);
        assert(err == 0);
        /*transformation*/
        Mat_t mat;
        bmp2Mat(&bmp, &mat);
        int step_size = mat.width * 32;
        for(int i=0,cnt=0; i<mat.height*mat.width; i+=step_size, cnt++){
            uint8_t * array = (uint8_t*)malloc(step_size * mat.cn *sizeof(uint8_t));
            memset(array, 0, step_size*mat.cn);
            Mat2Array_Partail(&mat,array,i);
            char * write_path;
            num_name_str(argv[3],cnt, write_path);
            Array_Save(write_path,array,step_size*mat.cn*sizeof(uint8_t));
            free(write_path);
            free(array);
        }
        Mat_free(&mat);
    }else if(opt == 2){
        //array2bmp
        BMP bmp, out_bmp;
        int32_t err = read_bmp(argv[2], &bmp);
        assert(err == 0);
        /*transformation*/
        Mat_t mat;
        bmp2Mat(&bmp, &mat);
        int step_size = mat.width * 32;
        for(int i=0; i<atoi(argv[3]);i++){
            uint8_t * array = (uint8_t*)malloc(step_size * mat.cn *sizeof(uint8_t));
            memset(array, 0, step_size*mat.cn);
            char * read_path;
            num_name_str(argv[4],i, read_path);
            Array_Read(read_path,array,step_size*mat.cn*sizeof(uint8_t));
            Array2Mat_Partial(&mat,array,step_size*i);
        }
        Mat2NewBmp(&mat, &bmp, &out_bmp);
        Mat_free(&mat);
        save_bmp(argv[5], &out_bmp);
    }else{
        printf("Error! opt should be 0 or 1\n");
    }

    return 0;
}

