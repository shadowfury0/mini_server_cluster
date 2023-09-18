#ifndef __L_NET_CLIENT__
#define __L_NET_CLIENT__

 #include "net_epoll.h"

class Net_Client{
public:
    Net_Client()
    {
        bzero(this->msgBuf,sizeof(this->msgBuf));
        connfd = -1;
        this->la_pos = 0;
        running = true;
    }
    ~Net_Client(){
    }
    void init(const char* ip,int port);
    void run();
    
    void downloadFile(const char* path);

    void lscmd();
    void getcmd(const  char* data);
    void othercmd();

    void recvPacket();
private:
    char ip[32];
    int port;
    int connfd;
    bool running;

    //缓冲区
    char msgBuf[sizeof(Packet) * 2];
    unsigned int la_pos;
    NetBuffer reader;
};

#endif