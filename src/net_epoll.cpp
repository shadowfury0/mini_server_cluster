#include "net_epoll.h"

void EpollMode::addfd(int fd,bool enable_et){
    epoll_event event;
    event.data.fd=fd;
    event.events=EPOLLIN|EPOLLOUT;//可读事件
    if(enable_et){//支持ET模式
        event.events|=EPOLLET;
        // event.events|=EPOLLONESHOT;//只被一个线程使用文件描述符
    }
    //添加事件
    if ( epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event) <  0 ) {
        std::cout << "epoll add error " << std::endl;
    }
    setnonblocking(fd);
}

void EpollMode::createEpollFD(){
    //先init在创建
    epollfd = epoll_create(5);//这里的5只是给内核一个提示事件表的大小无实际作用
    if (epollfd < 0){
        std::cout << "create epollfd error " << std::endl;
    }
} 

void EpollMode::delfd(int fd){
    epoll_event event;
    event.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&event);
}
int EpollMode::setnonblocking(int fd){//设置文件描述符为非阻塞
    int old_option=fcntl(fd,F_GETFL);
    int new_option=old_option|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;//返回旧描述符以便以后恢复
}

//检查用途
void EpollMode::event_trigger(struct epoll_event *e){
    
    if (e->events & EPOLLIN) {
        printf("EPOLLIN ");
    }
    if (e->events & EPOLLPRI){
        printf("EPOLLPRI ");
    }
    if (e->events & EPOLLOUT) {
        printf("EPOLLOUT ");
    }
    if (e->events & EPOLLRDNORM){
        printf("EPOLLRDNORM ");
    }
    if (e->events & EPOLLRDBAND){
        printf("EPOLLRDBAND ");
    }
    if (e->events & EPOLLWRBAND){
        printf("EPOLLWRBAND ");
    }
    if (e->events & EPOLLMSG){
        printf("EPOLLMSG ");
    }
    if (e->events & EPOLLERR){
        printf("EPOLLERR ");
    }
    if (e->events & EPOLLHUP){
        printf("EPOLLHUP ");
    }
    if (e->events & EPOLLRDHUP){
        printf("EPOLLRDHUP ");
    }
    if (e->events & EPOLLWAKEUP){
        printf("EPOLLWAKEUP ");
    }
    if (e->events & EPOLLONESHOT){
        printf("EPOLLONESHOT ");
    }
    if (e->events & EPOLLET){
        printf("EPOLLET ");
    }

    printf("\n");
} 