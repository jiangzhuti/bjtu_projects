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

#include "monitor_server.h"
#include "simplelog/libsimplelog/libsimplelog.h"

//受控进程列表
struct thread_proc_t ctrld_list[128];
//普通进程列表
struct thread_proc_t normal_list[128];
//时间服务请求列表
struct timesvc_client_list_t timesvc_cli_list;
//timesvc_cli_list 的读写锁
pthread_rwlock_t timesvc_rwlock;

volatile int ctrld_cnt, normal_cnt;//只增不减，不会出现访问到未初始化元素的情况，所以不用加锁。

struct sockaddr_un address;
int socket_fd, conn;
size_t addr_len;

int is_daemon;//是否以守护进程运行
#define IF_NOT_DAEMON if (!is_daemon)

static void print_proc(struct proc_t *proc)
{
    if (is_daemon) return;
    fprintf(stderr, "pid:%d\tcmdline:%s\tlct:%lu\n", proc->pid, proc->cmdline, proc->last_chk_time);
}

/*** 静态函数声明 ***/

//线程分四种:
//主线程、进程监控线程、时间服务线程和日志线程

/*** main thread ***/
/* 文件锁 */
static int lockfile(int fd);
/* 处理提交新进程的消息 */
static void process_new_proc(IN struct proc_t *proc);
/* 处理新的请求时间服务消息 */
static void process_new_time_request(IN struct req_time_t *req_time_info, int connfd);
/* 创建时间服务线程 */
static pthread_t create_time_service_thread();
/* 创建普通进程处理线程 */
static pthread_t new_normal_proc_thread(int proc_idx);
/* 创建受控进程处理线程 */
static pthread_t new_ctrld_proc_thread(int proc_idx);
/* 创建日志线程 */
static pthread_t create_log_thread();

/*** time svc thread ***/
/* 时间服务线程执行函数 */
static void *run_timesvc(void *arg);
/* 唤醒线程 */
static int wakeup_proc(int connfd);

/*** proc thread ***/
/* 判断进程是否存活 */
static int is_proc_alive(IN struct proc_t *proc);
/* 检查进程是否超时 */
static void check_proc_timed_out(IN OUT struct proc_t *proc);
/* 普通进程处理线程的执行函数 */
static void *run_normal(void *proc_arg);
/* 受控进程处理线程的执行函数 */
static void *run_ctrld(void *proc_arg);
/* 重启进程 */
static int restart_proc(struct proc_t *proc);

/*** log thread ***/
/* 日志线程执行函数 */
static void *run_log(void *arg);
/* 记录进程不存在 */
static int log_proc_dead(IN struct proc_t *proc);
/* 记录系统状态 */
static void log_status();


static int lockfile(int fd)
{
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return fcntl(fd, F_SETLK, &fl);
}

int already_running()
{
    extern int errno;
    int fd;
    char buf[16];

    fd = open(LOCK_FILE, O_RDWR | O_CREAT, LOCK_MODE);
    if (fd < 0) {
        fprintf(stderr, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {// already running.
            close(fd);
            return 1;
        }
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return 0;
}

void daemonize()
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask(0);

    int temp;
    temp = getrlimit(RLIMIT_NOFILE, &rl);
    if (temp < 0) {
        fprintf(stderr, "can't get file limit.\n");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "can't fork.\n");
        exit(EXIT_FAILURE);
    } else if (pid != 0) {
        exit(EXIT_SUCCESS);
    }
    setsid();
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    temp = sigaction(SIGHUP, &sa, NULL);
    if (temp < 0) {
        fprintf(stderr, "can't ignore SIGHUP");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "can't fork");
        exit(EXIT_FAILURE);
    } else if (pid != 0)
        exit(EXIT_SUCCESS);
    temp = chdir("/");
    if (temp < 0) {
        fprintf(stderr, "can't change directoy to /");
        exit(EXIT_FAILURE);
    }
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        fprintf(stderr, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(EXIT_FAILURE);
    }
}

static pthread_t create_log_thread()
{
    pthread_t thread;
    pthread_create(&thread, NULL, run_log, NULL);
    return thread;
}

pthread_t start_time_svc()
{
    pthread_rwlock_init(&timesvc_rwlock, NULL);
    INIT_LIST_HEAD(&timesvc_cli_list.head);
    create_time_service_thread();
    return create_time_service_thread();
}

static pthread_t create_time_service_thread()
{
    pthread_t thread;
    pthread_create(&thread, NULL, run_timesvc, NULL);
    return thread;
}

int serv_listen()
{
    int fd, len, err, rval;
    struct sockaddr_un un;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) , 0)
        return -1;
    unlink(SOCKET_PATH);
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, SOCKET_PATH);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(SOCKET_PATH);
    if (bind(fd, (struct sockaddr *)&un, len) < 0) {
        rval = -2;
        goto errout;
    }
    if (listen(fd, QLEN) , 0) {
        rval = -3;
        goto errout;
    }
    return fd;

errout:
    err = errno;
    close(fd);
    errno = err;
    return rval;
}

int serv_accept(int listenfd)
{
    int clifd;
    if ((clifd = accept(listenfd, NULL, NULL)) < 0)
        return -1;
    return clifd;
}

void serv_rcv_action(int connfd)
{
    struct msg_t msg;
    int i, cnt;
    recvfrom(connfd, &msg, sizeof(msg), 0, NULL, 0);
    switch (msg.msg_type) {
    case NEW_PROC:
        print_proc(&msg.msg_body.proc);
        process_new_proc(&msg.msg_body.proc);
        close(connfd);
        break;
    case HEARTBEAT_SIGNAL:
        IF_NOT_DAEMON fprintf(stderr, "recv heartbeat from pid: %d\n", msg.msg_body.heartbeat_pid);
        cnt = ctrld_cnt;
        for (i = 0; i < cnt; i++) {
            if (ctrld_list[i].proc.pid == msg.msg_body.heartbeat_pid) {
                pthread_rwlock_wrlock(&ctrld_list[i].rwlock);
                ctrld_list[i].proc.last_chk_time = time(NULL);
                pthread_rwlock_unlock(&ctrld_list[i].rwlock);
            }
            cnt = ctrld_cnt;
        }
        close(connfd);
        break;
    case REQUEST_TIME_SRV:
        IF_NOT_DAEMON fprintf(stderr, "timesvc request from pid:%d , tid: %lu\n", msg.msg_body.req_time.pid, msg.msg_body.req_time.tid);
        process_new_time_request(&msg.msg_body.req_time, connfd);
        break;
    default:
        //something was wrong..
        break;
    }
}

static void process_new_proc(struct proc_t *proc) //不用加锁，因为XXX_cnt还没更新
{
    print_proc(proc);
    pthread_t tid;
    //初始化工作全部放在主线程里，防止线程同步带来的问题
    if (proc->type == NORMAL) {
        memcpy(&(normal_list[normal_cnt].proc), proc, sizeof(*proc));
        pthread_rwlock_init(&(normal_list[normal_cnt].rwlock), NULL);
        tid = new_normal_proc_thread(normal_cnt);
        normal_list[normal_cnt].tid = tid;
        normal_cnt++;
    } else if (proc->type == CONTROLED) {
        memcpy(&(ctrld_list[ctrld_cnt].proc), proc, sizeof(*proc));
        pthread_rwlock_init(&(ctrld_list[ctrld_cnt].rwlock), NULL);
        ctrld_list[ctrld_cnt].proc.last_chk_time = time(NULL);
        tid = new_ctrld_proc_thread(ctrld_cnt);
        ctrld_list[ctrld_cnt].tid = tid;
        ctrld_cnt++;
    } else {//something was wrong...

    }
}

static pthread_t new_normal_proc_thread(int proc_idx)
{
    pthread_t thread;
    pthread_create(&thread, NULL, run_normal, &(normal_list[proc_idx].proc));
    return thread;
}

static pthread_t new_ctrld_proc_thread(int proc_idx)
{
    pthread_t thread;
    pthread_create(&thread, NULL, run_ctrld, &(ctrld_list[proc_idx].proc));
    return thread;

}

static void process_new_time_request(struct req_time_t *req_time_info, int connfd)
{
    list_head *pos, *n;
    struct timesvc_client_list_t *tmp;
    pthread_rwlock_rdlock(&timesvc_rwlock);
    //check duplicate error
    list_for_each_safe(pos, n, &timesvc_cli_list.head) {
        if (list_entry(pos, struct timesvc_client_list_t, head)->cli.pid == req_time_info->pid &&
            list_entry(pos, struct timesvc_client_list_t, head)->cli.tid== req_time_info->tid) {
            //error
            pthread_rwlock_unlock(&timesvc_rwlock);
            return;
        }
    }
    pthread_rwlock_unlock(&timesvc_rwlock);
    tmp = malloc(sizeof(*tmp));
    tmp->cli.pid = req_time_info->pid;
    tmp->cli.type = req_time_info->type;
    tmp->cli.wakeup_time = req_time_info->wakeup_time;
    tmp->cli.begin_time = req_time_info->begin_time;
    tmp->cli.connfd = connfd;
    pthread_rwlock_wrlock(&timesvc_rwlock);
    list_add_tail(&tmp->head, &timesvc_cli_list.head);
    pthread_rwlock_unlock(&timesvc_rwlock);
}

static void *run_timesvc(void *arg)
{
    list_head *pos, *n;
    struct timesvc_client_list_t *entry;

    (void) (arg);//消除编译时无用参数警告

    while (1) {
        pthread_rwlock_wrlock(&timesvc_rwlock);
        list_for_each_safe(pos, n, &timesvc_cli_list.head) {
            entry = list_entry(pos, struct timesvc_client_list_t, head);
            //fprintf(stderr, "checking pid %d, type %d, time %lu, begin_time %lu\n", entry->req_time_info.pid, entry->req_time_info.time_type, entry->req_time_info.wakeup_time, entry->begin_time);
            switch (entry->cli.type) {
            case ABSOLUTE:
                if (difftime(time(NULL), entry->cli.wakeup_time) >= 0) {
                    wakeup_proc(entry->cli.connfd);
                    close(entry->cli.connfd);
                    list_del(pos);
                    free(entry);
                }
                break;
            case RELATIVE:
                if (difftime(time(NULL), entry->cli.wakeup_time + entry->cli.begin_time) >= 0) {
                    IF_NOT_DAEMON fprintf(stderr, "df %f\n", difftime(time(NULL), entry->cli.wakeup_time + entry->cli.begin_time));
                    wakeup_proc(entry->cli.connfd);
                    close(entry->cli.connfd);
                    list_del(pos);
                    free(entry);
                }
                break;
            default:
                //error
                break;
            }
        }
        pthread_rwlock_unlock(&timesvc_rwlock);
        sleep(1);
    }
    return NULL;
}

static int wakeup_proc(int connfd)
{
    int i = 0;
    sendto(connfd, &i, sizeof(i), 0, NULL, 0);
    return 1;
}

static int is_proc_alive(IN struct proc_t *proc)
{
    if (kill(proc->pid, 0) < 0) {
        return 0;
    } else {
        return 1;
    }
}

static void check_proc_timed_out(IN OUT struct proc_t *proc)
{
    IF_NOT_DAEMON fprintf(stderr, "checking proc: %d\n", proc->pid);
    if (!is_proc_alive(proc)) {
        //log_proc_dead(proc);
        IF_NOT_DAEMON fprintf(stderr, "proc %d is stopped.trying to start it.\n", proc->pid);
        if (difftime(time(NULL), proc->last_chk_time) >= 10) {
            restart_proc(proc);
            IF_NOT_DAEMON fprintf(stderr, "proc restarted.now its pid is: %d\n", proc->pid);
        }
    } else {
        IF_NOT_DAEMON fprintf(stderr, "proc %d is alive.\n", proc->pid);
        pthread_rwlock_wrlock(&(CONTAINER_OF(proc, struct thread_proc_t, proc)->rwlock));
        proc->last_chk_time = time(NULL);
        pthread_rwlock_unlock(&(CONTAINER_OF(proc, struct thread_proc_t, proc)->rwlock));
    }
}

static void *run_normal(void *proc_arg)
{
    int index;//参数类型限制,只能这样获得数组下标了
    index = INDEX_OF(normal_list, CONTAINER_OF((struct proc_t *)proc_arg, struct thread_proc_t, proc));
    IF_NOT_DAEMON fprintf(stderr, "new thread created for proc: pid = %d, cmdline = %s at index: %d\n", normal_list[index].proc.pid, normal_list[index].proc.cmdline, index);
    while (1) {
        check_proc_timed_out(&(normal_list[index].proc));
        sleep(10);
    }
    return NULL;
}

static void *run_ctrld(void *proc_arg)
{
    int index;
    index = INDEX_OF(ctrld_list, CONTAINER_OF((struct proc_t *)proc_arg, struct thread_proc_t, proc));
    print_proc(proc_arg);
    while (1) {
        IF_NOT_DAEMON fprintf(stderr, "checking %s\n", ctrld_list[index].proc.cmdline);
        print_proc(&ctrld_list[index].proc);
        if (difftime(time(NULL), ctrld_list[index].proc.last_chk_time) >= 10) {
            IF_NOT_DAEMON fprintf(stderr, "%f, restarting...\n", difftime(time(NULL), ctrld_list[index].proc.last_chk_time));
            if (is_proc_alive(&(ctrld_list[index].proc))) {
                IF_NOT_DAEMON fprintf(stderr, "still alive\n");
                kill(ctrld_list[index].proc.pid, SIGKILL);
            } else {
		log_proc_dead(&ctrld_list[index].proc);
            }
            restart_proc(&ctrld_list[index].proc);
        }
        sleep(2);
    }
    return NULL;
}


static int restart_proc(struct proc_t *proc)
{
    int pid;
    time_t starttime;
    pid = exec_cmdline(proc->cmdline);
    starttime = time(NULL);
    pthread_rwlock_wrlock(&(CONTAINER_OF(proc, struct thread_proc_t, proc)->rwlock));
    proc->pid = pid;
    proc->last_chk_time = proc->start_time = starttime;
    pthread_rwlock_unlock(&(CONTAINER_OF(proc, struct thread_proc_t, proc)->rwlock));

    return 1;
}

pthread_t start_log()
{
    return create_log_thread();
}

static int log_proc_dead(struct proc_t *proc)
{
    static char *msg = "proc dead";
    log_add_unit(STRING_TYPE, strlen(msg) + 1, msg);
    log_add_unit(UINT_TYPE, sizeof(unsigned int), &(proc->pid));
    log_add_unit(STRING_TYPE, strlen(proc->cmdline) + 1, proc->cmdline);
    log_write(LOG_FILE);
    return 1;
}

static void *run_log(void *arg)
{
    (void) (arg);
    while (1) {
        sleep(10);
        log_status();
    }
    return NULL;
}

static void log_status()
{
    list_head *pos;
    struct timesvc_client_list_t *entry;
    static char *head = "***log_status start***";
    static char *foot = "***log_status end***";
    int i;
    time_t t;
    int cnt = 0;
    log_add_unit(STRING_TYPE, strlen(head) + 1, head);
    log_write(LOG_FILE);
    cnt = normal_cnt;
    // normal proc
    for (i = 0; i < cnt; i++) {
        pthread_rwlock_rdlock(&normal_list[i].rwlock);
        t = time(NULL);
        log_add_unit(ULONG_TYPE, sizeof(t), &t);//record start time
        log_add_unit(UINT_TYPE, sizeof(unsigned int), &normal_list[i].proc.pid);//pid
        log_add_unit(INT_TYPE, sizeof(int), &normal_list[i].proc.type);//proc type
        log_add_unit(STRING_TYPE, strlen(normal_list[i].proc.cmdline), normal_list[i].proc.cmdline);//cmdline
        pthread_rwlock_unlock(&normal_list[i].rwlock);
        log_write(LOG_FILE);
        cnt = normal_cnt;
    }
    cnt = ctrld_cnt;
    // ctrld proc
    for (i = 0; i < cnt; i++) {
        pthread_rwlock_rdlock(&ctrld_list[i].rwlock);
        t = time(NULL);
        log_add_unit(ULONG_TYPE, sizeof(t), &t);//record start time
        log_add_unit(UINT_TYPE, sizeof(unsigned int), &ctrld_list[i].proc.pid);//pid
        log_add_unit(INT_TYPE, sizeof(int), &ctrld_list[i].proc.type);//proc type
        log_add_unit(STRING_TYPE, strlen(ctrld_list[i].proc.cmdline), ctrld_list[i].proc.cmdline);//cmdline
        pthread_rwlock_unlock(&ctrld_list[i].rwlock);
        log_write(LOG_FILE);
        cnt = ctrld_cnt;
    }
    //timesvc
    pthread_rwlock_rdlock(&timesvc_rwlock);
    list_for_each(pos, &timesvc_cli_list.head) {
        entry = list_entry(pos, struct timesvc_client_list_t, head);
        log_add_unit(UINT_TYPE, sizeof(unsigned int), &(entry->cli.pid));//pid
        log_add_unit(ULONG_TYPE, sizeof(unsigned long), &(entry->cli.tid));//tid
        log_add_unit(INT_TYPE, sizeof(int), &(entry->cli.type));//type
        log_add_unit(ULONG_TYPE, sizeof(unsigned long), &(entry->cli.begin_time));//begin time
        log_add_unit(ULONG_TYPE, sizeof(unsigned long), &(entry->cli.wakeup_time));// wakeup time
        log_write(LOG_FILE);
    }
    pthread_rwlock_unlock(&timesvc_rwlock);
    log_add_unit(STRING_TYPE, strlen(foot) + 1, foot);
    log_write(LOG_FILE);
}
