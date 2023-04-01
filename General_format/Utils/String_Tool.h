#ifndef STRING_TOOL_H
#define STRING_TOOL_H
#include<string.h>
#include<stdlib.h>
#include<assert.h>


void itoa(char* str, int num){
    int idx=0, mod=num%10, div=num;
    while(div!=0){
        str[idx++] = mod - '0';
        div = div/10;
        mod = div%10;
    }
    char tmp = str[0];
    for(int i=0;i<2;i++){
        str[i] = str[3-i];
        str[3-i] = tmp;
        tmp = str[i+1];
    }
}

//num maximum is 9999
void num_name_str(const char* name, int num, char* rstr){
    assert(num<9999);
    int name_len =strlen(name);
    char num_str[4] = {'0','0','0','0'};
    rstr = (char*)malloc((name_len+5+4)*sizeof(char));
    memset(rstr, 0 ,(name_len+5+4));
    itoa(num_str, num);
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
            rstr[i+1] = 'i';
            rstr[i+1] = 'n';
        }
    }
}

#endif