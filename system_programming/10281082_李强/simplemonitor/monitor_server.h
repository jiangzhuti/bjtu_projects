#ifndef MONITOR_SERVER_H
#define MONITOR_SERVER_H

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

#include "monitor_def.h"
#include "monitor_common.h"

/* 检测正在运行的服务端 */
int already_running();
/* 变成守护进程 */
void daemonize();
/* 开始监听 */
int serv_listen();
/* 接受连接 */
int serv_accept(int listenfd);
/* 处理请求 */
void serv_rcv_action(int connfd);
/* 开启时间服务 */
pthread_t start_time_svc();
/* 开启日志功能 */
pthread_t start_log();
#endif // MONITOR_SERVER_H
