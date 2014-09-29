#ifndef SIMPLEFS_H
#define SIMPLEFS_H

#define E_FILE_NOT_FOUND      (-1)
#define E_FILE_ALREADY_EXISTS (-2)
#define E_NO_SPACE_LEFT       (-3)
#define E_MAX_OPEN            (-4)
#define E_FILE_NOT_OPENED     (-5)
#define E_SF_LOAD_FAILED      (-6)
#define E_SF_STORE_FAILED     (-7)
#define E_SF_MKFS_FAILED      (-8)
#define E_UNKNOWN             (-9)

#define SF_EOF -1

typedef int sf_file;

int sf_mkfs(const char *file);
int sf_load(const char *file);
int sf_store(const char *file);
int sf_create(const char *file_name);
int sf_destroy(const char *file_name);
sf_file sf_open(const char *file_name);
int sf_close(sf_file file);
unsigned sf_read(sf_file file, void *mem_area, unsigned count);
unsigned sf_write(sf_file file, const void *mem_area, unsigned count);
#define SF_SEEK_CUR 0
#define SF_SEEK_SET 1
#define SF_SEEK_END 2
void sf_lseek(sf_file file, int len, int whence);
void sf_directory();
const char *sf_err(int errid);

#endif // SIMPLEFS_H
