#ifndef MONITOR_DEF_H
#define MONITOR_DEF_H
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
#include "list_head.h"

#define bool char
#define true (char)0
#define false (char)1

/* 指针参数类型 */
#define IN
#define OUT

#define MAX_PROC_PATH 256
#define MAX_CMDLINE 256

/* 日志文件 */
#define LOG_FILE "log.log"

#define LOCK_FILE "/tmp/monitor_daemon.pid"
#define LOCK_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FIFO_MODE (S_IRUSR | S_IWUSR | S_IWGRP | S_IROTH | S_IWOTH)

#define SOCKET_PATH "/tmp/simple_monitor-socket"
#define QLEN 10

/* 调试用的 */
#define FUNC_ENTER fprintf(stderr, "ENTERING FUNCTION: %s\n", __func__);
#define FUNC_LEAVE fprintf(stderr, "LEAVING FUNCTION: %s\n", __func__);

#define CONTAINER_OF(ptr, type, member) ({   \
          const typeof( ((type *)0)->member ) *__mptr = (ptr); \
          (type *)( (char *)__mptr - offsetof(type,member) );})

#define INDEX_OF(array, ptr) \
    (((ptr) - (array)) / sizeof(*ptr))

//进程类型
enum proc_type {
    NORMAL,//普通进程
    CONTROLED//受控进程
};

//进程结构
struct proc_t {
    char path[MAX_PROC_PATH];//进程路径
    char cmdline[MAX_CMDLINE];//进程命令行
    pid_t pid;//进程标识符
    time_t start_time;//进程开始时间
    time_t last_chk_time;//最后一次检查进程存在的时间或者最后一次收到心跳信号时间
    enum proc_type type;//普通还是受控
};
//消息类型
enum msg_type {
    NEW_PROC,//提交新进程
    HEARTBEAT_SIGNAL,//心跳信号
    REQUEST_TIME_SRV,//请求时间服务
};
//时间服务类型
enum timesvc_type {
  ABSOLUTE,//绝对时间
  RELATIVE//相对时间
};
//请求时间服务结构
struct req_time_t {
    enum timesvc_type type;//请求类型
    time_t begin_time;//开始时间
    time_t wakeup_time;//唤醒时间
    pid_t pid;//进程标识符
    pthread_t tid;//线程标识符
};
//消息结构
struct msg_t {
    enum msg_type msg_type;//消息类型
    union {
        struct proc_t proc;
        struct req_time_t req_time;
        pid_t heartbeat_pid;
    } msg_body;//消息内容
};
//时间服务客户端结构
struct timesvc_client_t {
    enum timesvc_type type;//请求类型
    time_t begin_time;//开始时间
    time_t wakeup_time;//唤醒时间
    pid_t pid;//进程标识符
    pthread_t tid;//线程标识符
    int connfd;//连接描述符
};
//时间服务客户端链表
struct timesvc_client_list_t {
    list_head head;//双向链表
    struct timesvc_client_t cli;//客户端
};

struct thread_proc_t {
    pthread_t tid;//处理线程
    struct proc_t proc;//客户进程
    pthread_rwlock_t rwlock; // 需要加锁的情况：每个元素初始化在主线程完成，然后，除了所属的处理线程会访问到，只有log线程记录日志时需要读取。
                             // 元素一旦创建，对log线程来说，只有proc.pid、proc.start_time、proc.last_chk_time
                             // 会被改变。因此，这三处改变的地方需要在处理线程内同时加锁解锁（写锁），而在log线程内加读锁。
                             // 一个例外是主线程会对heartbeat进行处理，更新last_chk_time，需要写加锁。
};



#endif
