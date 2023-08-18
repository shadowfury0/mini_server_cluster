#ifndef __NET_SERVER_CLUSTER__
#define __NET_SERVER_CLUSTER__

#include "net_server.h"

inline void networker(Net_Server& tmp){
    tmp.run();
}

class ServerCluster{
public:
    ServerCluster():
    server_count(NULL),
    server_cl(NULL),
    startPort(0)
    {}
    ~ServerCluster(){
        if (server_count != NULL){
            delete server_count;
        }
        if (server_cl != NULL){
            delete server_cl;
        }
    }

    void init(int n,int startPort);
    void join();

private:
    //地址暂时用本地
    const char* ipaddr = "127.0.0.1";
    int startPort;
    int w_num;
    std::thread* server_count;
    Net_Server* server_cl;
};


#endif