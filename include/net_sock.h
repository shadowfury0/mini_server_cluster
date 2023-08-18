#ifndef __L_NET_SOCKET__
#define __L_NET_SOCKET__

#include "net_header.h"


//绑定服务器端口 socket对象
class Socket{
public:
    Socket(){
        listenfd = -1;
    }
    virtual ~Socket(){
        if (listenfd >= 0){
            close(listenfd);
        }
    }
    //ip 地址 port端口  max最大连接数
    void init(const char* ip,int port,int max = 5,bool istcp = true);

public:  
    int listenfd;
};

#endif