#include "net_server_cluster.h"


void ServerCluster::init(const char* ip,int startPort,int n){
    w_num = n;
    server_count = new std::thread[n];
    
    server_cl = new Net_Server[n];
    ipaddr = ip;
    //create子线程
    for (size_t i = 0; i < w_num; i++)
    {
        server_cl[i].init(ipaddr,i + startPort);
    }
    
    for (size_t i = 0; i < w_num; i++)
    {
        server_count[i] = std::thread(networker,std::ref(server_cl[i]));
    }
}
void ServerCluster::join(){
    for (size_t i = 0; i < w_num; i++)
    {
        server_count[i].join();
    }
}