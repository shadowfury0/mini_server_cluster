#include <iostream>

#include "net_buffer.h"

int main(){

    NetBuffer wr;

    wr.push("mybad");
    char* my = wr.pop();

    printf("%s\n",my);

    free(my);
    return 0;
}