#ifndef CONTROLED_PROC_H
#define CONTROLED_PROC_H
#include "../monitor_def.h"

int cli_conn();
int cli_send_msg(IN struct msg_t *msg, int connfd);

int request_time_srv();


#endif // CONTROLED_PROC_H
