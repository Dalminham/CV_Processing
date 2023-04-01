#ifndef _INCLUDE_ARRAY_IO_H
#define _INCLUDE_ARRAY_IO_H

#include <stdio.h>
#include <stdint.h>

void Array_Save(const char* file_path, uint8_t* ptr, int size)
{
    FILE *fp = NULL;
    fp = fopen(file_path,"w");
    fwrite(ptr, size, 1, fp);
    fclose(fp);
}

void Array_Read(const char* file_path, uint8_t* array, int size)
{
    FILE *fp = NULL;
    if (NULL == file_path)
    {
        printf("Wrong File Path!!!\n");
        return;
    }
    fp = fopen(file_path,"rb");
    if (NULL == fp)
    {
        printf("Empty File!!!\n");
        return;
    }

    fread(array, size, 1, fp);
}

void Array_Print(uint8_t* array, int size)
{
    for(int i=0; i<size; i++)
    {
        printf("%d,\n",array[i]);
        if(i%64 ==0)
        printf("\n");
    }
}

#endif