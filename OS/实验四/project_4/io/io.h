#ifndef IO_H
#define IO_H

#define BLOCK_COUNT 1024
#define BLOCK_SIZE  128

int read_block(int blkid, void *buf);
int write_block(int blkid, void *buf);
int save_to_file(const char *filename);
int load_from_file(const char *filename);

#endif // IO_H
