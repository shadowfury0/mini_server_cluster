#ifndef __L_NET_EPOLL__
#define __L_NET_EPOLL__

#include "net_header.h"
#include "net_buffer.h"

class EpollMode{
public:
    EpollMode():epollfd(-1){
    }
        
    ~EpollMode(){
        if (events != NULL){
            free(events);
        }
        if (epollfd >= 0){
            close(epollfd);
        }
    }

    void init(){
        init(sizeof(Packet),sizeof(Packet) );
    }
    void init(int max_e_num,int buf_s){
        max_event_num = max_e_num;
        buffer_size = buf_s;
        events = new epoll_event[max_e_num];
        if (NULL == events ){
            std::cout << "events allocate error " << std::endl;
        }
        createEpollFD();
    }
    //添加文件描述符fd到事件表epollfd中 
    void addfd(int fd,bool enable_et);
    void addfd(int fd){ addfd(fd,false); }
    //删除文件描述符fd
    void delfd(int fd);

    void createEpollFD();
    //文件描述符非阻塞模式
    int setnonblocking(int fd);
    //判断epoll哪类事件触发
    void event_trigger(struct epoll_event *e);
public:
    int epollfd;
    size_t max_event_num;//epoll最大个数
    size_t buffer_size;//一次读取数据大小
    epoll_event* events = NULL;
};


//lt水平触发
// void lt(epoll_event* events,int number,int epollfd,int listenfd){//LT模式
//     char buf[BUFFER_SIZE];
//     for(int i=0;i<number;i++){//就绪事件
//         int sockfd=events[i].data.fd;
//         if(sockfd==listenfd){//若为服务端监听端口表示事件(有新连接)发生则接受连接
//             struct sockaddr_in client_address;
//             socklen_t client_addrlength=sizeof(client_address);
//             int connfd=accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength);
//             addfd(epollfd,connfd,false);//将新连接添加到事件表且设置为非ET
//         }
//         else if(events[i].events&EPOLLIN){//客户端连接事件(有数据发送到服务端)发生则服务端接收数据
//             std::cout<<"lt  trigger "<<std::endl;
//             pool.enqueue(readBuffer,buf,sockfd);
//         }
//         else{
//             std::cout<<"something wrong"<<std::endl;
//         }
//     }
// }


// void et(epoll_event* events,int number,int epollfd,int listenfd){//ET模式
//     char* buf = new char[ep.buffer_size];
//     try{
//         for(int i=0;i<number;i++){//就绪事件
//             int sockfd=events[i].data.fd;
//             if(sockfd==listenfd){//监听端口有新连接
//                 struct sockaddr_in client_address;
//                 socklen_t client_addrlength=sizeof(client_address);
//                 int connfd=accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength);
//                 ep.addfd(epollfd,connfd,true);//新连接设置为ET模式
//             }
//             else if(events[i].events&EPOLLIN){//客户端有数据发送到服务端
//                 cout<<"et trigger once"<<endl;
//                 // readBuf(buf,sockfd);
//                 pl.enqueue(readBuf,buf,sockfd);
//             }
//             else{
//                 cout<<"somethig wrong"<<endl;
//             }
//         }
//     }catch(...){
//         delete buf;
//     }
//     delete buf;
// }
#endif