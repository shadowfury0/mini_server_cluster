
#include "net_server_cluster.h"

using namespace std;

#include <pthread.h>
#include <thread>

int main(int argc,char* argv[]){

    ServerCluster ser;
    ser.init(1,9999);
    ser.join();

    // sleep(100);
    return 0;
}