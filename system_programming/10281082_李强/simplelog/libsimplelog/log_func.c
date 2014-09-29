#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "log_func.h"

char log_file[256];
static int log_fd;

DLL_LOCAL void open_log_file(const char *logfile)
{
    extern int errno;
    strcpy(log_file, logfile);
    if ((log_fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND, FILE_MODE)) < 0) {
        fprintf(stderr, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

DLL_LOCAL int close_log_file()
{
    return close(log_fd);
}

static inline off_t get_cur_pos(int fd)
{
    return lseek(fd, 0, SEEK_CUR);
}

DLL_LOCAL int write_log(void *buf, int len, OUT off_t *pos)
{
    if (write(log_fd, buf, len) < 0) return 0;
    if (pos == NULL) return 0;
    *pos = get_cur_pos(log_fd) - len;
    return 1;
}

