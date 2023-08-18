#ifndef __L_NET_SERVER__
#define __L_NET_SERVER__

#include "net_epoll.h"
#include "net_sock.h"
#include "net_buffer.h"

#include "threadPool.h"


class Net_Server{
public:
    Net_Server() :
    ep(NULL),
    sock(NULL){
    }
    ~Net_Server(){ 
        if (sock != NULL )  {
            delete sock;
        }
        if (ep != NULL ){
            delete ep;
        }
    }

    void run();

    void init(const char* ip,int startPort);
private:
    void runChild();

    //暂时
    void receive(char* buf,int sockfd);
    void post(int sockfd);

    void parsecmd(int sockfd);

    void et(epoll_event* events,int number,int listenfd);
private:
    const char* ipaddr;
    int port;
    bool running = true;

    // ThreadPool<>* pl;//线程池
    ThreadPool netpool;

    Socket* sock;
    EpollMode* ep;//epoll模式

    //读缓冲区
    NetBuffer read_buf;
    //写缓冲区
    NetBuffer write_buf;
};



#endif