#ifndef MONITOR_CLIENT_H
#define MONITOR_CLIENT_H

#include "monitor_def.h"
#include "monitor_common.h"
int cli_conn();
int cli_send_msg(IN struct msg_t *msg, int connfd);

#endif // MONITOR_CLIENT_H
