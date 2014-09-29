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

#include "monitor_client.h"


//finished with apue
int cli_conn()
{
    int fd, len, err, rval;
    struct sockaddr_un un;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) , 0)
        return -1;
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, SOCKET_PATH);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(SOCKET_PATH);
    if (connect(fd, (struct sockaddr *)&un, len) < 0) {
        rval = -4;
        goto errout;
    }
    return fd;
errout:
    err = errno;
    close(fd);
    errno = err;
    return rval;
}

int cli_send_msg(IN struct msg_t *msg, int connfd)
{
    sendto(connfd, msg, sizeof(*msg), 0, NULL, 0);
    return 1;
}
