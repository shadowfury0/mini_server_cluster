#ifndef __L_NET_CLIENT__
#define __L_NET_CLIENT__

 #include "net_epoll.h"
 #include "net_buffer.h"

class Net_Client{
public:
    Net_Client(){
        connfd = -1;
        running = true;
    }
    ~Net_Client(){
        
    }
    void init(const char* ip,int port,int max = 5);
    void run();
private:
    int connfd;
    bool running;
};

#endif