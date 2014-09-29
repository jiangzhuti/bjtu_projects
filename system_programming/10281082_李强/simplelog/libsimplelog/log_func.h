#ifndef LOG_FUNC_H
#define LOG_FUNC_H

#include "log_def.h"

/* 打开/创建日志文件 */
void open_log_file(const char *logfile);

/* 关闭日志文件 */
int close_log_file();

/* 写日志 */
int write_log(void *buf, int len, OUT off_t *pos);

#endif // LOG_FUNC_H
