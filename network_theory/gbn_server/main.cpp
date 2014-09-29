#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "shell_server.h"
#include "gobackn_protocol.h"
#include "udp_connection/server_udpconnection.h"

int main(int argc,char **argv) {

    int port;
    if (argc <= 1) {
        //printf("usage: udp-server <port>\n");
        printf("using defeault port %i\n", DEFAULT_PORT);
        port = DEFAULT_PORT;
    } else {
        port = atoi(argv[1]);
    }
    ServerUDPConnection c(port);
    gbn_protocol p;
    shell_server server(&p, &c);
    server.start();
    return 0;
}
