#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "shell_server.h"
#include "gobackn_protocol.h"
#include "udp_connection/server_udpconnection.h"

shell_server::shell_server(Protocol *p, Connection *c) {
	this->p = p;
	this->c = c;
}

void shell_server::start() {
	if (c->connect() < 0) {
		printf("Could not connect\n");
		return;
	}
	p->setConnection(c);
	char *mesg;
	while ((mesg = p->receiveMessage()) != NULL) {
		fputs(mesg,stdout);
		printf("\n");
	}
	printf("server stopping\n");
}
