#ifndef __L_NET_CLIENT__
#define __L_NET_CLIENT__

 #include "net_epoll.h"
 #include "net_buffer.h"

class Net_Client{
public:
    Net_Client()
    {
        connfd = -1;
        running = true;
    }
    ~Net_Client(){
    }
    void init(const char* ip,int port);
    void run();
    
private:
    char ip[32];
    int port;
    int connfd;
    bool running;

};

#endif