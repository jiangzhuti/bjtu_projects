#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "shell_client.h"
#include "gobackn_protocol.h"
#include "udp_connection/client_udpconnection.h"

shell_client::shell_client(Protocol *p, Connection *c) {
	this->p = p;
	this->c = c;
}

void shell_client::start() {

	if (c->connect() < 0) {
		printf("Could not connect\n");
		return;
	}
	p->setConnection(c);
    while (printf("Input Mesg: ") && gets(send_line) != NULL) {
        p->sendMessage(send_line, strlen(send_line));
        printf("Application Layer: MESSAGE '%s' SENT\n", send_line);
	}
	printf("stdin returned NULL\n");
}
