#include <iostream>

#include "net_client.h"

int main(){
    Net_Client client;
    client.init("127.0.0.1",9999);
    client.run();

    return 0;
}