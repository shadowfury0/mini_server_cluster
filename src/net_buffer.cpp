#include "net_buffer.h"

#include <stdlib.h>

int NetBuffer::getSize(){
    return buf.size();
}

void NetBuffer::push(const Packet& packet){
    // size_t len = strlen(data);
    Packet* tmp = NULL; //需要回收内存
    //内存及其不安全给他多预留一个空间,给'\0'使用，防止内存访问错误
    tmp = new Packet();
    
    *tmp = packet;//复制
    
    lock.lock();
    buf.push_back(tmp);
    lock.unlock();

}
//需要回收指针指向内存
Packet* NetBuffer::pop(){
    Packet* tmp;
    tmp = buf.front();
    lock.lock();
    buf.pop_front();
    lock.unlock();
    return tmp; 
}
const Packet* NetBuffer::front(){
    return buf.front();
}
const Packet* NetBuffer::back(){
    return buf.back();
}

bool NetBuffer::isEmpty(){
    return buf.empty();
}

