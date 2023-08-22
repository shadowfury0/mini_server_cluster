#include "net_client.h"


void Net_Client::init(const char* ip,int port,int max){
    struct sockaddr_in address;
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
 
}
void Net_Client::run(){
    char buff[256];
    char sbuff[256];

    while (running)
    {
        fgets(sbuff,sizeof(sbuff),stdin);
        send(connfd,sbuff,strlen(sbuff),0);
        if(!strcmp(sbuff,"quit\n") ){
            running = false;
        } 
        while (running)
        {
            bzero(buff,strlen(buff));
            int ret = recv(connfd, buff, sizeof(buff) , 0);
            if ( 0 == ret ){
                printf("server missing connnection\n");
                break;
            }else if (ret > 0) {
                printf("%s", buff);
            }else{
                break;
            }
        }
        
    }
}