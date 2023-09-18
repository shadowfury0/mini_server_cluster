
#include "net_server_cluster.h"

using namespace std;

#include <pthread.h>
#include <thread>

int main(int argc,char* argv[]){

    if ( argc <= 2){
        printf(" usage: %s ip_address port_number backlog \n",basename(argv[0]));
        return 0;    
    } 
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    ServerCluster ser;
    ser.init(ip,port,1);
    ser.join();

    // sleep(100);
    return 0;
}