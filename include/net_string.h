#ifndef __NET_STRING__
#define __NET_STRING__

#include <string.h>
#include <string>

//删除首尾空字符串,不会改变原有字符串,返回栈变量
static inline char* strTrim(const char* str){
    int len = strlen(str);
    char* tmp = new char[len + 1];

    int i = 0;
    while(str[i] == ' ' || str[i] ==  '\t' ){
        i++;
    }
    
    int j = len;
    while (str[j] == ' ' || str[j] == '\0' || str[j] == '\n' || str[j] == '\t')
    {
        j--;
    }
    int t = 0;
    for ( int k = i; k <= j ; ) {
        tmp[t++] = str[k++];
    }
    tmp[t] = '\0';
    return tmp;
}


#endif