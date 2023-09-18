#include <iostream>

#include "net_client.h"

int main(int argc,char* argv[]){
    if ( argc <= 2){
        printf(" usage: %s ip_address port_number backlog \n",basename(argv[0]));
        return 0;    
    } 
    Net_Client client;

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    client.init(ip,port);
    client.run();

    return 0;
}