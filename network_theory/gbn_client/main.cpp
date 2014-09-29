#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "shell_client.h"
#include "gobackn_protocol.h"
#include "udp_connection/client_udpconnection.h"

int main(int argc,char **argv) {

    int port;
    if (argc <= 1) {
        printf("usage: udp-client <server_ip> <port>\n");
        exit(-1);
    } else if (argc <= 2) {
        port = DEFAULT_PORT;
    } else {
        port = atoi(argv[2]);
    }
    ClientUDPConnection c(port, argv[1]);
    gbn_protocol p;
    shell_client client(&p, &c);
    client.start();
    return 0;
}
