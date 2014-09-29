#ifndef INDEX_FUNC_H
#define INDEX_FUNC_H

/*为了保证简洁，此模块仅提供最基本的操作，没有读取日志和索引文件的功能，
 *文件描述符仅在模块内有效，所以对index加读锁的函数放在libsimplelog.c中。*/
int lock_index_file_w();
int unlock_index_file_w();

/* 打开/创建索引文件 */
void open_index_file(const char *indexfile);

/* 关闭日志索引文件 */
int close_index_file();

/* 写索引文件 */
void write_index(void *buf, int len);

#endif // INDEX_FUNC_H
