#ifndef __L_WRITE_BUFFER__
#define __L_WRITE_BUFFER__

#include <list>
#include <string.h>
#include "lock.h"

class NetBuffer{
public:
    NetBuffer(){
        lock.getLock();//init 
    }
    ~NetBuffer(){}

    void push(const char* data);

    char* pop();

    int getSize();

    bool isEmpty(); 
private:
    std::list<char*> buf;
    //加锁,默认互斥
    Lock<> lock;
};



#endif