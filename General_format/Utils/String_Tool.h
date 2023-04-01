#ifndef STRING_TOOL_H
#define STRING_TOOL_H
#include<string.h>
#include<stdlib.h>
#include<assert.h>


char* itoa(int num){
    char *str = (char*)malloc(4*sizeof(char));
    for(int i=0;i<4;i++)
        str[i] = '0';
    int idx=0, mod=num%10, div=num;
    while(div!=0){
        str[idx++] = mod + '0';
        div = div/10;
        mod = div%10;
    }
    char tmp = str[0];
    for(int i=0;i<2;i++){
        str[i] = str[3-i];
        str[3-i] = tmp;
        tmp = str[i+1];
    }
    return str;
}

//num maximum is 9999
char* num_name_str(const char* name, int num){
    assert(num<9999);
    int name_len =strlen(name);
    char* num_str = itoa(num);
    char *rstr = (char*)malloc((name_len+5+4)*sizeof(char));
    memset(rstr, 0 ,(name_len+5+4));
    for(int i=0, j=0;i<name_len+5+4;i++){
        if(i<name_len){
            rstr[i] = name[i];
        }else if(i == name_len){
            rstr[i] = '_';
        }else if(i < name_len + 5){
            rstr[i] = num_str[j++];
        }else{
            rstr[i] = '.';
            rstr[i+1] = 'b';
            rstr[i+2] = 'i';
            rstr[i+3] = 'n';
            break;
        }
    }
    return rstr;
}

#endif