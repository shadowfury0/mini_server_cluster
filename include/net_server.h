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
    sock(NULL),
    curDir(NULL)
    {
    }
    ~Net_Server(){ 
        if (sock != NULL )  {
            delete sock;
        }
        if (ep != NULL ){
            delete ep;
        }
        if (curDir != NULL){
            delete curDir;
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
    //访问头
    void preHead(int sockfd);
    //下载文件
    void downloadFile(const char* path);
    //如果输入负一则全部递归
    void lscmd(const char* path,int recur,int prin);
    void cdcmd(char* path);
    void getcmd(char* path);

    void netmode(epoll_event* events,int number,int listenfd,bool iset);
private:
    const char* ipaddr;
    char* curDir;

    int port;
    bool running = true;

    size_t c_char_len;//字符数组长度

    Lock<> s_lock;
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