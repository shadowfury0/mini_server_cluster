#include "net_buffer.h"

#include <stdlib.h>

int NetBuffer::getSize(){
    return buf.size();
}

void NetBuffer::push(const char* data){
    size_t len = strlen(data);
    char* tmp = NULL; //需要回收内存
    //内存及其不安全给他多预留一个空间,给'\0'使用，防止内存访问错误
    tmp = new char[len + 1];
    strcpy(tmp,data);
    
    lock.lock();
    buf.push_back(tmp);
    lock.unlock();

}
//需要回收指针指向内存
char* NetBuffer::pop(){
    char* tmp;
    tmp = buf.front();
    lock.lock();
    buf.pop_front();
    lock.unlock();
    return tmp; 
}

bool NetBuffer::isEmpty(){
    return buf.empty();
}