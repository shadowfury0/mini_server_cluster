#include "net_buffer.h"

#include <stdlib.h>

int NetBuffer::getSize(){
    return buf.size();
}

void NetBuffer::push(const char* data){
    size_t len = strlen(data);
    char* tmp = NULL; 
    tmp = (char*)malloc(sizeof(char) * len);
    strcpy(tmp,data);
    
    lock.lock();
    buf.push_back(tmp);
    lock.unlock();
}

//记得回收内存
char* NetBuffer::pop(){
    lock.lock();
    char* tmp = buf.front();
    buf.pop_front();
    lock.unlock();
    return tmp;
}

bool NetBuffer::isEmpty(){
    return buf.empty();
}