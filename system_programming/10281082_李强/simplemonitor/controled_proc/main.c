#include <stdio.h>
#include "../monitor_client.h"

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <stropts.h>
#include <stddef.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>

pid_t pid;

void *run_heartbeat(void *arg)
{
    struct msg_t msg;
    int connfd;
    (void) (arg);
    msg.msg_type = HEARTBEAT_SIGNAL;
    msg.msg_body.heartbeat_pid = pid;
    while (1) {
        connfd = cli_conn();
        //fprintf(stderr, "sending hs\n");
        cli_send_msg(&msg, connfd);
        close(connfd);
        sleep(2);
    }
    return NULL;
}

void *run_timesvc_cli(void *arg)
{
    struct msg_t msg;
    int connfd;
    int i;
    (void) (arg);
    msg.msg_type = REQUEST_TIME_SRV;
    msg.msg_body.req_time.pid = pid;
    msg.msg_body.req_time.begin_time = time(NULL);
    msg.msg_body.req_time.tid = pthread_self();
    msg.msg_body.req_time.wakeup_time = 10;
    msg.msg_body.req_time.type = RELATIVE;
    while (1) {
        connfd = cli_conn();
        fprintf(stderr, "request for time svc\n");
        cli_send_msg(&msg, connfd);
        fprintf(stderr, "sleeping\n");
        recvfrom(connfd, &i, sizeof(i), 0, NULL, 0);
        fprintf(stderr, "wake up\n");
        sleep(500);
        close(connfd);
    }
    return NULL;
}

int main(void)
{
    pthread_t trd_heartbeat;
    pthread_t trd_timesvc_cli;
    pid = getpid();
    pthread_create(&trd_heartbeat, NULL, run_heartbeat, NULL);
    pthread_create(&trd_timesvc_cli, NULL, run_timesvc_cli, NULL);
    pthread_join(trd_heartbeat, NULL);
    pthread_join(trd_timesvc_cli, NULL);
    return 0;
}

