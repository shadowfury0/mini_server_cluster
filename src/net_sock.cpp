#include "net_sock.h"

#include <sys/errno.h>

void Socket::init(const char* ip,int port,int max,bool istcp){
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family=AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr);
    address.sin_port=htons(port);

    
    
    if ( istcp ){
        if (-1 == (listenfd=socket(PF_INET,SOCK_STREAM,0)) )
        {
            printf("Create TCP socket error(%d): %s\n", errno, strerror(errno));
        }
        //网上说是为了 删除time_wait时间，不能绑定窗口进行的设置的问题
        //该方法可行
        int optval = 1;
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        if ( bind(listenfd,(struct sockaddr*)&address,sizeof(address)) == -1){
            printf("Bind error(%d): %s\n", errno, strerror(errno));
        }
        
        if ( -1 == listen(listenfd,max) ){
            printf("Listen error(%d): %s\n", errno, strerror(errno));
        }
    }else {
        if (-1 == (listenfd = socket(PF_INET,SOCK_DGRAM,0) ) )
        {
            printf("Create UDP socket error(%d): %s\n", errno, strerror(errno));
        }

        if ( bind(listenfd,(struct sockaddr*)&address,sizeof(address)) == -1){
            printf("Bind error(%d): %s\n", errno, strerror(errno));
        }
    }
}