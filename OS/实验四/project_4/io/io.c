#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

static unsigned char ldisk[BLOCK_COUNT][BLOCK_SIZE];
#define DISK_BEGIN ((unsigned char *)ldisk)
#define DISK_BLOCK(blk_num) \
    (DISK_BEGIN + (blk_num) * BLOCK_SIZE)

static inline int is_valid_block(unsigned i)
{
    return (i < BLOCK_COUNT);
}

int read_block(int blkid, void *buf)
{
    if (!is_valid_block(blkid)) return -1;
    memcpy(buf, DISK_BLOCK(blkid), BLOCK_SIZE);
    return 0;
}

int write_block(int blkid, void *buf)
{
    if (!is_valid_block(blkid)) return -1;
    memcpy(DISK_BLOCK(blkid), buf, BLOCK_SIZE);
    return 0;
}

int save_to_file(const char *filename)
{
    FILE *fp = fopen(filename, "w+");
    if (fp == NULL) return -1;
    if (fwrite(DISK_BEGIN, BLOCK_SIZE, BLOCK_COUNT, fp) != BLOCK_COUNT) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

int load_from_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return -1;
    if (fread(DISK_BEGIN, BLOCK_SIZE, BLOCK_COUNT, fp) != BLOCK_COUNT) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}
