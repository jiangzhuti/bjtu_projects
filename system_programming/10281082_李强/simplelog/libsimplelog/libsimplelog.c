#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "log_def.h"
#include "log_func.h"
#include "index_func.h"
#include "libsimplelog.h"

#ifdef __cplusplus
    extern "C" {
#endif

/* 存放每条日志的描述部分, used by log_add_unit() and log_write() */
static unsigned char desc_buffer[1024 * 4];
/* 日志单元的个数, unit_count * 2 == desc_len, used by log_add_unit() and log_write() */
static int unit_count;
/* 存放每条日志的内容部分, used by log_add_unit() and log_write() */
static unsigned char data_buffer[1024 * 4];
/* 日志内容部分的长度, used by log_add_unit() and log_write() */
static int data_len;
/* 读取到的日志内容 */
static struct log_unit view_log_record[1024];

/* add a unit to the buffer */
DLL_PUBLIC void log_add_unit(unsigned char utype, unsigned char ulen, void *ubuf)
{
    desc_buffer[unit_count * 2] = utype;
    desc_buffer[unit_count * 2 + 1] = ulen;
    unit_count++;
    memcpy(data_buffer + data_len, ubuf, ulen);
    data_len += ulen;
}

/* write the record buffer to log file */
DLL_PUBLIC void log_write(const char *logfile)
{
    static char indexfile[512];
    static off_t pos;
    static int desc_len;
    static int total_len;
    static time_t log_time;
    strcpy(indexfile, logfile);
    strcat(indexfile, ".index");
    desc_len = unit_count * 2;
    total_len = desc_len + data_len;
    open_index_file(indexfile);
    lock_index_file_w();
    open_log_file(logfile);
    write_log(desc_buffer, desc_len, &pos);
    write_log(data_buffer, data_len, NULL);
    close_log_file();

    log_time = time(NULL);
    write_index(&log_time, sizeof(log_time));
    write_index(&pos, sizeof(pos));
    write_index(&desc_len, sizeof(desc_len));
    write_index(&total_len, sizeof(total_len));
    unlock_index_file_w();
    close_index_file();
    unit_count = data_len = desc_len = total_len = 0;//reset
}

/* 转换字符串到time_t */
static time_t str2time_t(const char *timestr)
{
    static struct tm tmp_tm;
    strptime(timestr, "%Y-%m-%d:%H:%M:%S", &tmp_tm);
    return mktime(&tmp_tm);
}

/* 打印lu指向的一条记录, 记录的单元个数为len */
static void print_record(struct log_unit *lu, int len)
{
    static int i;
    char *strtab[] = {"%d", "%u", "%ld", "%lu", "%c", "%f", "%s"};
    for (i = 0; i < len; i++) {
        switch (lu[i].unit_type) {
            case INT_TYPE:printf(strtab[0], lu[i].unit.int_unit);break;
            case UINT_TYPE:printf(strtab[1], lu[i].unit.uint_unit);break;
            case LONG_TYPE:printf(strtab[2], lu[i].unit.long_unit);break;
            case ULONG_TYPE:printf(strtab[3], lu[i].unit.ulong_unit);break;
            case CHAR_TYPE:printf(strtab[4], lu[i].unit.char_unit);break;
            case FLOAT_TYPE:printf(strtab[5], lu[i].unit.float_unit);break;
            case STRING_TYPE:printf(strtab[6], lu[i].unit.string_unit);break;
            default:exit(EXIT_FAILURE);break;
        }
        putchar('\t');
    }
    printf("\b\n");
}

/* index read lock */
static int lock_index_file_r(int fd)
{
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    return fcntl(fd, F_SETLKW, &lock); 
}

/* unlock index */
static int unlock_index_file_r(int fd)
{
    struct flock lock;
    lock.l_type = F_ULOCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    return fcntl(fd, F_SETLK, &lock);
}

/* view logs from begin_time to end_time */
DLL_PUBLIC void log_view(const char *begin_time, const char *end_time, const char *logfile)
{
    static char indexfile[512];
    static off_t view_pos;
    static int view_desc_len;
    static int view_total_len;
    static time_t bt, et;
    static time_t ct;
    int index_fd, log_fd;
    int i;
    strcpy(indexfile, logfile);
    strcat(indexfile, ".index");
    if ((index_fd = open(indexfile, O_RDONLY)) < 0) {
        exit(EXIT_FAILURE);
    }
    lock_index_file_r(index_fd);
    if((log_fd = open(logfile, O_RDONLY)) < 0) {
        unlock_index_file_r(index_fd);
        close(index_fd);
        exit(EXIT_FAILURE);
    }
    bt = str2time_t(begin_time);
    et = str2time_t(end_time);
    if (read(index_fd, &ct, sizeof(ct)) <= 0) {
        unlock_index_file_r(index_fd);
        close(index_fd);
        close(log_fd);
        return;
    }
    /* 跳过begin_time以前的记录 */
    while (difftime(ct, bt) < 0) {
        lseek(index_fd, INDEX_RECORD_SIZE - sizeof(time_t), SEEK_CUR);
        if (read(index_fd, &ct, sizeof(ct)) <= 0) {
            unlock_index_file_r(index_fd);
            close(index_fd);
            close(log_fd);
            return;
        }
    }
    /* 读取[begin_time, end_time]之间的记录 */
    while (difftime(ct, et) <= 0) {
        read(index_fd, &view_pos, sizeof(view_pos));
        read(index_fd, &view_desc_len, sizeof(view_desc_len));
        read(index_fd, &view_total_len, sizeof(view_total_len));
        lseek(log_fd, view_pos, SEEK_SET);
        for (i = 0; i < view_desc_len / 2; i++) {
            read(log_fd, &view_log_record[i].unit_type, sizeof(view_log_record[i].unit_type));
            read(log_fd, &view_log_record[i].unit_len, sizeof(view_log_record[i].unit_len));
        }
        for (i = 0; i < view_desc_len / 2; i++) {
            read(log_fd, &view_log_record[i].unit, view_log_record[i].unit_len);
        }
        print_record(view_log_record, view_desc_len / 2);
        if (read(index_fd, &ct, sizeof(ct)) <= 0) {
            unlock_index_file_r(index_fd);
            close(index_fd);
            close(log_fd);
            return;
        }
    }
    unlock_index_file_r(index_fd);
    close(index_fd);
    close(log_fd);
}

#ifdef __cplusplus
}
#endif
