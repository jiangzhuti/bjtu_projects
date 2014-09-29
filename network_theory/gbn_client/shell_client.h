#ifndef _SHELL_CLIENT_H
#define _SHELL_CLIENT_H

#include "udp_connection/common.h"

class shell_client {
private:
	Protocol *p;
	Connection *c;

    char send_line[BUF_SIZE];
	
public:
    shell_client(Protocol *p, Connection *c);
	void start();
};
#endif
