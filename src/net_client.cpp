#include "net_client.h"

void Net_Client::init(const char* ip,int port){
    strcpy(this->ip,ip);
    this->port = port;

    struct sockaddr_in address;
    //暂时以tcp为主
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == connfd)
    {
        printf("Create socket error(%d): %s\n", errno, strerror(errno));
        running = false;
        return;
    }

    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr);
    address.sin_port = htons(port);

    if (-1 == connect(connfd,(struct sockaddr*)&address,sizeof(address) )){
        printf("Connect error(%d): %s\n", errno, strerror(errno));
        running = false;
        return;
    }
}

void Net_Client::lscmd(){
    for(;;){
        recvPacket();
        //判断收到数据
        if ( -1 == ntohl(reader.back()->len) ) {
            break;
        }
    }

    while ( !reader.isEmpty() )
    {
        Packet* msg = NULL;
        msg = reader.pop();

        printf("%s",msg->buf);
        if (msg != NULL){
            delete msg;
        }
    }
}

void Net_Client::downloadFile(const char* path){
    FILE* fp;
    fp = fopen(path, "w");
    if (fp == NULL) {
        std::cout << " client open file error " << std::endl;
        return ;
    }

    for(;;){
        recvPacket();
        if (-1 == reader.back()->len) {
            break;
        }
    }

    while ( !reader.isEmpty() )
    {
        Packet* msg = NULL;
        msg = reader.pop();

        fputs(msg->buf,fp);
        fflush(fp);
        
        // printf("%s",msg->buf);
        if (msg != NULL){
            delete msg;
        }
    }

    if (fclose(fp) != 0) {
        std::cout << "client close file error " << std::endl;
        return ;
    }
}

void Net_Client::getcmd(const char* data){
    char* tmp = NULL;
    tmp = strTrim(data);

    char* token = strchr(tmp,' ');//第一个参数

    if (token == NULL || tmp == "" ){
        if (tmp != NULL){
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

    token = strchr(tmp2,' ');//第二个参数
    char* tmp3 = strTrim(token);

    if ( token == NULL || tmp3 == "" ) {
        std::cout << " args no equal three " << std::endl;
        if (tmp != NULL){
            free(tmp);
            tmp = NULL;
        }
        if (tmp2 != NULL){
            free(tmp2);
            tmp2 = NULL;
        }
        if (tmp3 != NULL){
            free(tmp3);
            tmp3 = NULL;
        }
        return;
    }

    Packet pack;

    pack = data;

    send(connfd,(Packet*)&pack,sizeof(pack),0);

    downloadFile(tmp3);//直接下载

    if (tmp != NULL){
        free(tmp);
        tmp = NULL;
    }
    if (tmp2 != NULL){
        free(tmp2);
        tmp2 = NULL;
    }
    if (tmp3 != NULL){
        free(tmp3);
        tmp3 = NULL;
    }
    
}

void Net_Client::run(){
    char buff[sizeof(Packet)];
    Packet write;

    while (running)
    {
        printf("mini_ftp -> ");
        bzero(buff,0);
        fgets(buff,sizeof(buff),stdin);

        bzero(msgBuf,sizeof(msgBuf));
        la_pos = 0;

        write = buff;
        //判断命令
        if ( !strncmp(buff,"ls",2) || !strncmp(buff,"dir",2) ) {
            send(connfd,(Packet*)&write,sizeof(write),0);
            lscmd();
        }
        else if ( !strncmp(buff,"get",3) ) {
            getcmd(buff);
        } 
        else {
            send(connfd,(Packet*)&write,sizeof(write),0);

            othercmd();
        }
        write.pzero();
    }

    close(connfd);
}

void Net_Client::othercmd(){
    int ret = recv(connfd,this->msgBuf,sizeof(Packet),0);
    Packet packet;

    memcpy(&packet,this->msgBuf,sizeof(Packet));

    printf("%s\n",packet.buf);
}

void Net_Client::recvPacket(){
    int p_len = sizeof(Packet);
    char r_buf[ p_len ];

    int ret = recv(this->connfd,r_buf,sizeof(Packet), 0);
    if ( ret <= 0 ){
        return ;
    }
    //拷贝数据
    memcpy(msgBuf + this->la_pos ,r_buf,ret);
    this->la_pos += ret ;
    if ( this->la_pos < p_len ){
        return;
    }
    //接收数据
    Packet pack;
    bzero(r_buf,p_len);
    memcpy(&pack,this->msgBuf,p_len);

    //数据前移
    memcpy(this->msgBuf,this->msgBuf + p_len ,this->la_pos - p_len);
    this->la_pos -= p_len;
    reader.push(pack);
}