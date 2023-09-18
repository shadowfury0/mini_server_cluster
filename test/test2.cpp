#include <iostream>
#include <string.h>

using namespace std;
int main(){

    char* path = "wwqw";

    if ( !strncmp(path,"asd",3) ){
        std::cout << "hello" << endl;
    }
    return 0;
}