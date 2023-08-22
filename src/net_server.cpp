#include "net_server.h"

#include <string.h>
#include <dirent.h>

//先暂时这样格式化条件太多
//格式化字符串
inline void strformat(char* tmp,const char* name,int reverse){
    strcat(tmp,"|");
    //效率很低到时候进行更改
    for(int i = 0;i< ( reverse - 1 )* 2 ;i++){
        strcat(tmp," ");
    }
    //一级目录不打印
    if (reverse != 1){
        strcat(tmp,"|");
    }
    strcat(tmp,"__");
    strcat(tmp,name);
    strcat(tmp,"\n");
}

void Net_Server::lscmd(const char* path,int recur,int prin,int sockfd){
    int ret;

    ret = access(path,F_OK);
    if (ret < 0){
        std::cout << " dir not exit" << std::endl;
        return;
    }
    
    struct dirent *fd;
    DIR *dir = opendir(path);

    if (NULL == dir){
        perror("Error opening dir\n");
        return;
    }
    
    while ((fd = readdir(dir)) != NULL) {
        if (0 == recur){
            return;
        }
        // 忽略当前目录和父目录
        if (strcmp(fd->d_name, ".") == 0 || strcmp(fd->d_name, "..") == 0) {
            continue;
        }
        
        char tmp[c_char_len];
        memset(tmp,'\0',sizeof(tmp));
        if (fd->d_type == DT_DIR){
            //想想办法变小一点
            char fname[c_char_len];  //获取文件名

            // printf("%d : dir -- > %s\n", recur,fd->d_name);
            sprintf(fname,"%s/%s",path,fd->d_name)//strange

            strformat(tmp,fd->d_name,prin);

            write_buf.push(tmp);
            std::cout <<"get in : " << fname << std::endl;
            lscmd(fname,recur - 1,prin + 1,sockfd);
        }else{ 
            strformat(tmp,fd->d_name,prin);

            write_buf.push(tmp);
        }
    }
    
    closedir(dir);//关闭
}
//太复杂了
void Net_Server::cdcmd(char* path){
    //截取字符串
    char* token = strtok(path, " ");
    token = strtok(NULL," ");//取第二个目录值

    //这个应该要加锁
    // s_lock.lock();
    memset(curDir,'\0',strlen(curDir));
    strncpy(curDir,token,strlen(token) - 1);

    write_buf.push(curDir);
    write_buf.push("\n");

    // s_lock.unlock();//这里解锁目的是防止死锁
}
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
void Net_Server::preHead(int sockfd){
    write_buf.push("mini_ftp >");
    post(sockfd);
}
void Net_Server::parsecmd(int sockfd){
    char* line_t = read_buf.pop();

    //一定要加\n因为发送数据都是回车
    if (!strcmp(line_t,"hello\n")){
        write_buf.push("hello my friend \n");
    }
    else if(!strcmp(line_t,"pwd\n")){
        // std::cout << curDir << std::endl;
        write_buf.push(curDir);
        write_buf.push("\n");
    }
    else if(!strncmp(line_t,"cd",2)){
        cdcmd(line_t);
    }
    else if (!strcmp(line_t,"ls\n") || !strcmp(line_t,"dir\n")){
        write_buf.push(curDir);
        write_buf.push("\n");
        //查看当前文件目录
        lscmd(curDir,3,1,sockfd);
    }
    
    post(sockfd);
    preHead(sockfd);
    
    //删除
    if (line_t != NULL){
        free(line_t);
        line_t = NULL;
    }
}

void Net_Server::post(int sockfd){
    char* t_ch;
    while (!write_buf.isEmpty())
    {
        t_ch = write_buf.pop();
        send(sockfd,t_ch,strlen(t_ch),0);

        if (t_ch != NULL){
            free(t_ch);
            t_ch = NULL;
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

            preHead(connfd);
        }
        else if(events->events & EPOLLIN){//客户端有数据发送到服务端
            std::cout<<"et trigger once"<<std::endl;
            char* buf = NULL; 
            buf = new char[ep->buffer_size];                
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

    c_char_len = 256; // 32字节
    //当前文件夹路径
    //这里是栈变量要更改
    curDir = NULL;
    curDir = (char*)malloc(sizeof(char) * c_char_len);
    strcpy(curDir,"/opt/network");

    //加锁
    s_lock.getLock();

    int i ;
    sock = NULL;
    sock = new Socket();
    sock->init(ipaddr,port);

    ep = NULL;
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
    