#ifndef _SHELL_SERVER_H
#define _SHELL_SERVER_H

#include "udp_connection/common.h"

class shell_server {
private:
	Protocol *p;
	Connection *c;

public:
    shell_server(Protocol *p, Connection *c);
	void start();
};

#endif
