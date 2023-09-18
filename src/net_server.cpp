#include "net_server.h"

#include <dirent.h>

//先暂时这样格式化条件太多
//格式化字符串
void Net_Server::strformat(char* tmp,const char* name,int reverse){
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

//暂定这种读文件方式，以后一更高效的读取文件的方式再进行替换
void Net_Server::readFile(const char* path){
    FILE* fp;

    char line[PACK_BUFF_SIZE];//到时候修改
    fp = fopen(path, "r");//目前绝对路径
    if (fp == NULL) {
        Packet pack;

        pack = "file open error\n";
        write_buf.push(pack);
        return ;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        Packet pack;
        strcpy(pack.buf,line);
        pack.len = strlen(line);

        write_buf.push(pack);
        pack.pzero();
    }

    // 关闭文件
    if (fclose(fp) != 0) {
        Packet pack;

        pack = "file close error\n";
        write_buf.push(pack);
        return ;
    }
}

void Net_Server::lscmd(const char* path,int recur,int prin){
    int ret;
    Packet pack;
    ret = access(path,F_OK);
    if (ret < 0){
        pack = " dir not exit \n";
        std::cout << pack.buf << std::endl;
        write_buf.push(pack);
        return;
    }
    
    struct dirent *fd;
    DIR *dir = opendir(path);

    if (NULL == dir){
        pack = "Error opening dir\n";
        write_buf.push(pack);
        perror("Error opening dir\n");
        closedir(dir);
        return;
    }
    
    while ((fd = readdir(dir)) != NULL) {
        if (0 == recur){
            closedir(dir);
            return;
        }
        // 忽略当前目录和父目录
        if (strcmp(fd->d_name, ".") == 0 || strcmp(fd->d_name, "..") == 0) {
            continue;
        }
        
        char tmp[PACK_BUFF_SIZE];
        memset(tmp,'\0',sizeof(tmp));
        if (fd->d_type == DT_DIR){
            //想想办法变小一点
            char fname[PACK_BUFF_SIZE];  //获取文件名

            // printf("%d : dir -- > %s\n", recur,fd->d_name);
            sprintf(fname,"%s/%s",path,fd->d_name);//strange

            strformat(tmp,fd->d_name,prin);
            
            pack = tmp;
            write_buf.push(pack);
            lscmd(fname,recur - 1,prin + 1);
            
        }else{ 
            strformat(tmp,fd->d_name,prin);

            pack = tmp;
            write_buf.push(pack);
        }
    }
    
    closedir(dir);//关闭
}

void Net_Server::cdcmd(char* path){
    Packet packet;

    //截取字符串
    char* token = strtok(path, " ");
    token = strtok(NULL," ");//取第二个目录值

    if (strtok(NULL," ")){
        //判断有无第三个参数
        std::cout << "error args larger than 2 " << std::endl;
        // write_buf.push("error args larger than 2\n");
        packet = "error args larger than 2 ";
        write_buf.push(packet);
        return;
    }
    //这个应该要加锁
    s_lock.lock();
    memset(curDir,'\0',strlen(curDir));
    strncpy(curDir,token,strlen(token) - 1);
    s_lock.unlock();//这里解锁目的是防止死锁

    packet = curDir;
    write_buf.push(packet);

}

void Net_Server::getcmd(char* path){    
    char* tmp = NULL;;
    tmp = strTrim(path);

    char* token = strchr(tmp,' ');//第一个参数

    if (token == NULL || tmp == "" ){
        if (tmp!= NULL){
            free(tmp);
            tmp = NULL;
        }
        return;
    }

    char* tmp2  = strTrim(token);
    if ( token == NULL || tmp2 == ""){
        std::cout << "args no equal three " << std::endl;
        if (tmp != NULL){
            free(tmp);
            tmp = NULL;
        }
        if (tmp2 != NULL){
            free(tmp2);
            tmp2 = NULL;
        }
        return;
    }

    char* tmp3 = strtok(tmp2," ");
    //下载文件并发送
    readFile(tmp3);

    if (tmp != NULL){
        free(tmp);
        tmp = NULL;
    }
    if (tmp2 != NULL){
        free(tmp2);
        tmp2 = NULL;
    }
    
}
//暂时放这里测试
void Net_Server::receive(char* buf,int sockfd){
    try{
        printf("thread  id : %u\n",pthread_self());
        //这里到时候给while设置一个全局变量接受失败就退出
        while(1){//这里要一次性读完客户端发送来的数据因为ET是不会重复触发
            memset(buf,'\0',ep->buffer_size);
            int ret=recv(sockfd,buf,ep->buffer_size ,0);
            if(ret<0){//非阻塞IO中recv返回-1并不一定是出错
                if((errno==EAGAIN)||(errno==EWOULDBLOCK)){//errno为EAGAIN和EWOULDBLOCK时表示数据已经读取完毕可以进行下一次epoll
                    std::cout<<"epoll again"<<std::endl;
                    break;
                }
                close(sockfd);
                break;
            }
            else if(0 == ret){//连接已关闭
                std::cout << "one connection close " << std::endl;
                close(sockfd);
            }
            else{
                //这里处理接收数据端的逻辑
                // std::cout<<"receive data:"<<ret<<" "<<buf<<std::endl;//输出接收到的数据
                std::cout<<"receive data:"<<ret<<" "<<std::endl;//输出接收到的数据
                //解析发来的数据
                Packet pack;
                pack = *(Packet*)buf;//转换长度

                printf("%s\n",pack.buf);
                read_buf.push(pack);
                parsecmd(sockfd);
                
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
    Packet* line_t = read_buf.pop();
    
    Packet pack;
    //一定要加\n因为发送数据都是回车
    if (!strcmp(line_t->buf,"hello\n")){
        pack = "hello my friend " ;
        write_buf.push(pack);
    }
    else if(!strcmp(line_t->buf,"pwd\n")){
        pack = curDir;
        write_buf.push(pack);
    }
    else if(!strncmp(line_t->buf,"cd",2)){
        cdcmd(line_t->buf);
    }
    else if (!strcmp(line_t->buf,"ls\n") || !strcmp(line_t->buf,"dir\n")){
        //查看当前文件目录
        lscmd(curDir,3,1);
        //发送数据长度为0的包
        pack = "";
        pack.len = -1;
        write_buf.push(pack);
    }
    else if(!strncmp(line_t->buf,"get",3)){
        getcmd(line_t->buf);

        pack = "";
        pack.len = -1;
        write_buf.push(pack);
    }
    else{
        pack = " nothing happened " ;
        write_buf.push(pack);
    }
    
    post(sockfd);
    
    //删除
    if (line_t != NULL){
        free(line_t);
        line_t = NULL;
    }
}

void Net_Server::post(int sockfd){
    Packet* t_ch;
    while (!write_buf.isEmpty())
    {
        t_ch = write_buf.pop();
        send(sockfd,(Packet*)&*t_ch,sizeof(Packet),0);
        usleep(1);//消息发送过快导致缓冲区爆满，容易丢包

        if (t_ch != NULL){
            free(t_ch);
            t_ch = NULL;
        }
    }
}

void Net_Server::netmode(epoll_event* events,int number,int listenfd,bool iset){//ET模式
    
    for(int i=0;i<number;i++){//就绪事件
        int sockfd=events->data.fd;
        //触发测试
        // ep->event_trigger(events);
        if(sockfd == listenfd){//监听端口有新连接
            struct sockaddr_in client_address;
            socklen_t client_addrlength=sizeof(client_address);
            int connfd=accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength);
            ep->addfd(connfd,iset);//新连接设置为ET模式
            
            std::cout << " listening address : " <<  inet_ntoa(client_address.sin_addr)
                << "-----> port : " << ntohs(client_address.sin_port) << std::endl;

        }
        else if(events->events & EPOLLIN){//客户端有数据发送到服务端
            std::cout<<" trigger once"<<std::endl;
            char* buf = NULL; 
            buf = new char[ep->buffer_size];                
            netpool.enqueue(&Net_Server::receive,this,buf,sockfd);
        }
        else {
            // std::cout<<"  somethig wrong  "<<std::endl;
        }
    }
}
inline void handle_pipe(int sig)
{
//do nothing
    // std::cout << "one connection close " << std::endl;
}

void Net_Server::init(const char* ip,int p){
    ipaddr = ip;
    port = p;

    //当前文件夹路径
    //这里是栈变量要更改
    curDir = NULL;
    curDir = (char*)malloc(sizeof(char) * PACK_BUFF_SIZE);
    strcpy(curDir,"/");

    //加锁
    s_lock.getLock();

    int i ;
    sock = new Socket();
    sock->init(ipaddr,port);

    ep = new EpollMode();
    ep->init();

    ep->addfd(sock->listenfd,true);

    netpool.init(1);

    //处理信号事件暂时搁置在这里
    struct sigaction sa;
    sa.sa_handler = handle_pipe;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGPIPE,&sa,NULL);
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
        //采用LT模式 改为false
        netmode(ep->events,ret,sock->listenfd,true);//采用ET模式
    }
}

void Net_Server::run(){
    //跑一个可以
    //跑循环无法创建线程
    runChild();
}
    