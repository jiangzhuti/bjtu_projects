#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "log_def.h"

char index_file[512];
static int index_fd;

DLL_LOCAL int lock_index_file_w()
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    return (fcntl(index_fd, F_SETLKW, &lock));
}

DLL_LOCAL int unlock_index_file_w()
{
    struct flock lock;
    lock.l_type = F_ULOCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    return (fcntl(index_fd, F_SETLK, &lock));
}

DLL_LOCAL void open_index_file(const char *indexfile)
{
    extern int errno;
    strcpy(index_file, indexfile);
    if ((index_fd = open(index_file, O_WRONLY | O_APPEND | O_CREAT, FILE_MODE)) < 0) {
        fprintf(stderr, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

DLL_LOCAL int close_index_file()
{
    return close(index_fd);
}

DLL_LOCAL void write_index(void *buf, int len)
{
    write(index_fd, buf, len);
}
