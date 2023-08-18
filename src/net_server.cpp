#include "net_server.h"

#include <string.h>
//暂时放这里测试
void Net_Server::receive(char* buf,int sockfd){
    try{
        printf("thread  id : %u\n",pthread_self());
        while(1){//这里要一次性读完客户端发送来的数据因为ET是不会重复触发
            memset(buf,'\0',ep->buffer_size);
            int ret=recv(sockfd,buf,ep->buffer_size + 4,0);
            if(ret<0){//非阻塞IO中recv返回-1并不一定是出错
                if((errno==EAGAIN)||(errno==EWOULDBLOCK)){//errno为EAGAIN和EWOULDBLOCK时表示数据已经读取完毕可以进行下一次epoll
                    std::cout<<"epoll again"<<std::endl;
                    break;
                }
                close(sockfd);
                break;
            }
            else if(ret==0){//连接已关闭
                std::cout << "one connection close " << std::endl;
                close(sockfd);
            }
            else{
                std::cout<<"receive data:"<<ret<<" "<<buf<<std::endl;//输出接收到的数据
                //解析发来的数据

                read_buf.push(buf);
                parsecmd(sockfd);
                // write_buf.push(buf);
                //建议直接发送会好一点
                // send(sockfd,buf,ret,0);
            }
        }
    }catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        std::cout << "thread running error " << std::endl;
        if(buf != NULL){
            delete buf;
        }
    }
    //回收
    if(buf != NULL){
        delete buf;
    }
}

void Net_Server::parsecmd(int sockfd){
    char* line_t = read_buf.pop();

    //一定要加\n因为发送数据都是回车
    if (!strcmp(line_t,"quit\n")){
        write_buf.push("server quit\n");
        post(sockfd);
        //断开对方连接
        shutdown(sockfd,SHUT_RDWR);
        
    }else if (!strcmp(line_t,"hello\n")){
        write_buf.push("hello my friend \n");
        post(sockfd);
    }

    if (line_t != NULL){
        free(line_t);
    }
}

void Net_Server::post(int sockfd){
    char* t_ch;
    while (!write_buf.isEmpty())
    {
        t_ch = write_buf.pop();
        send(sockfd,t_ch,strlen(t_ch),0);
        if (t_ch != NULL){
            free(t_ch);//释放内存
        }
    }
}

void Net_Server::et(epoll_event* events,int number,int listenfd){//ET模式
    
    for(int i=0;i<number;i++){//就绪事件
        int sockfd=events->data.fd;
        //触发测试
        // ep->event_trigger(events);
        if(sockfd==listenfd){//监听端口有新连接
            struct sockaddr_in client_address;
            socklen_t client_addrlength=sizeof(client_address);
            int connfd=accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength);
            ep->addfd(connfd,true);//新连接设置为ET模式
            
            std::cout << " listening address : " <<  inet_ntoa(client_address.sin_addr)
                << "-----> port : " << ntohs(client_address.sin_port) << std::endl;
        }
        else if(events->events & EPOLLIN){//客户端有数据发送到服务端
            std::cout<<"et trigger once"<<std::endl;
            // receive(buf,sockfd);
            char* buf = NULL; 
            buf = new char[ep->buffer_size];                
            // receive(buf,sockfd);
            netpool.enqueue(&Net_Server::receive,this,buf,sockfd);
        }
        else {
            std::cout<<"  somethig wrong  "<<std::endl;
        }
    }
}

void Net_Server::init(const char* ip,int p){
    ipaddr = ip;
    port = p;

    int i ;
    sock = new Socket();
    sock->init(ipaddr,port);

    ep = new EpollMode();
    ep->init();

    ep->addfd(sock->listenfd,true);

    netpool.init(3);
}
void Net_Server::runChild(){
    //跑监听事件
    while(running){
        //  int ret=epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);//等待事件就绪
        int ret=epoll_wait(ep->epollfd,ep->events,ep->max_event_num ,-1);//等待事件就绪
        if(ret < 0){
            //测试
            std::cout << "error  epoll wait"<< std::endl;
            break;
        }
        // lt(events,ret,epollfd,listenfd);//采用LT模式
        et(ep->events,ret,sock->listenfd);//采用ET模式
    }
}

void Net_Server::run(){
    //跑一个可以
    //跑循环无法创建线程
    runChild();
}
    