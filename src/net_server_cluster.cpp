#include "net_server_cluster.h"


void ServerCluster::init(int n,int startPort){
    w_num = n;
    server_count = new std::thread[n];
    
    server_cl = new Net_Server[n];
    
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