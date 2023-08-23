#include "net_client.h"


void Net_Client::init(const char* ip,int port){

    strcpy(this->ip,ip);
    this->port = port;

}
void Net_Client::run(){
   struct sockaddr_in address;
    char buff[2048];
    char sbuff[256];

    //暂时以tcp为主

    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == connfd)
    {
        printf("Create socket error(%d): %s\n", errno, strerror(errno));
        return;
    } 
        
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr);
    address.sin_port = htons(port);

    if (-1 == connect(connfd,(struct sockaddr*)&address,sizeof(address) )){
        printf("Connect error(%d): %s\n", errno, strerror(errno));
        return;
    }
    
    while (running)
    {
        while(recv(connfd, buff, sizeof(buff),MSG_DONTWAIT ) != -1 ) {
            printf("%s",buff);
            //当收到EOF时客户端知道文件传输已完成，跳出循环
            if (!strcmp(buff,"mini_ftp >")){
                memset(sbuff,'\0',sizeof(sbuff));
                fgets(sbuff,sizeof(sbuff),stdin);
                send(connfd,sbuff,strlen(sbuff),0);
            }
            memset(buff,'\0',sizeof(buff));
        }
    }
    close(connfd);
}