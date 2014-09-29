#include <signal.h>
#include <getopt.h>
#include "monitor_client.h"
#include "monitor_server.h"
#include "simplelog/libsimplelog/libsimplelog.h"

void start_svc()
{
    int listenfd, connfd;
    if ((listenfd = serv_listen()) < 0) {
	fprintf(stderr, "serv_listen failed!\n");
        exit(EXIT_FAILURE);
    }
    start_log();
    start_time_svc();
    while (1) {
        connfd = serv_accept(listenfd);
        if (connfd < 0) {
            fprintf(stderr, "serv_accept failed!\n");
            exit(EXIT_FAILURE);
        }
        serv_rcv_action(connfd);//serv_rcv_action会最终执行close(connfd)
    }
}

void start_daemon()
{
    extern int is_daemon;
    daemonize();
    if (already_running()) {
        //fprintf(stderr, "daemon already running!\n");
        exit(EXIT_FAILURE);
    }
    is_daemon = 1;
    start_svc();
}

void stop_daemon()
{
    exit(EXIT_SUCCESS);
}

//finished
void start_client(const char *cmdline, int type)
{
    struct msg_t msg;
    //struct proc_t proc;
    int connfd;
    pid_t pid;
    msg.msg_body.proc.type = type == 0 ? NORMAL : CONTROLED;
    strcpy(msg.msg_body.proc.cmdline, cmdline);
    connfd = cli_conn();
    if (connfd < 0) {
        fprintf(stderr, "connection failed!\n");
        exit(EXIT_FAILURE);
    }
    printf("connected to server.\n");
    printf("executing command..\n");
    pid = exec_cmdline(cmdline);
    msg.msg_body.proc.start_time = msg.msg_body.proc.last_chk_time = time(NULL);
    if (pid == 0) {
        //error
        exit(EXIT_FAILURE);
    }
    msg.msg_body.proc.pid = pid;
    printf("proc created. sending proc info...\n");
    msg.msg_type = NEW_PROC;
    cli_send_msg(&msg, connfd);
    printf("done.\n");
}

//finished
int main(int argc, char **argv)
{
    int arg;
    int cli = 0;
    int type = -1;
    char cmdline[512];
    char from[64], to[64], tmp[128];
    while ((arg = getopt(argc, argv, "dskc:t:v:")) != -1) {
        switch (arg) {
        case 'd':
            start_daemon();
            return 0;
	case 's':
	    start_svc();
	    return 0;
        case 'c':
            cli = 1;
            strcpy(cmdline, optarg);
            break;
        case 't':
            type = atoi(optarg);
            break;
        case 'k':
            stop_daemon();
            return 0;
        case 'v':
            strcpy(tmp, optarg);
            strcpy(from, strtok(tmp, ","));
            strcpy(to, strtok(NULL, ","));
            //printf("%s %s\n", from, to);
            log_view(from, to, LOG_FILE);
        default:
            printf("%c\n", arg);
            exit(EXIT_FAILURE);
        }
    }
    if (cli == 1) {
        if (type == -1) {
            fprintf(stderr, "select type!\n");
            exit(EXIT_FAILURE);
        }
        start_client(cmdline, type);
    }
    return 0;
}
