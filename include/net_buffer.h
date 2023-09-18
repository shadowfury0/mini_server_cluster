#ifndef __L_WRITE_BUFFER__
#define __L_WRITE_BUFFER__

#include <list>
#include <string.h>
#include "lock.h"

//定义包长度
#define PACK_BUFF_SIZE 499

struct Packet_Header{
    unsigned int len;
};
//508字节
struct Packet : public Packet_Header{
    char buf[PACK_BUFF_SIZE + 1];
    Packet(){
        pzero();
    }
    ~Packet(){}
    void operator=(const Packet& p){
        this->len = p.len;
        // this->offset = p.offset;
        strcpy(buf,p.buf);
    }
    void operator=(const char* buf){
        this->len = strlen(buf);
        // this->offset = this->len;
        strcpy(this->buf,buf);
    }
    void pzero(){
        this->len = 0;
        // this->offset = 0;
        // memset(buf,sizeof(buf),'\0');
        bzero(buf,sizeof(buf));
    }
};

class NetBuffer{
public:
    NetBuffer(){
        lock.getLock();//init 
    }
    ~NetBuffer(){

    }

    void push(const Packet& packet);

    Packet* pop();

    const Packet* front();
    const Packet* back();
    
    int getSize();

    bool isEmpty(); 
private:
    std::list<Packet*> buf;
    //加锁,默认互斥
    Lock<> lock;
};


#endif