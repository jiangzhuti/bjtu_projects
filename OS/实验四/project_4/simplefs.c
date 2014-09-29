#include "simplefs.h"
#include "io/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESERVED_BLOCK_CNT 16

/* FD
 *---+---------------+--------------+-----------------+-----------------+---+--------------------------+------------
 *...|tag_used(uchar)|file_len(uint)|file_blk[0](uint)|file_blk[1](uint)|...|file_blk[FBLK_CNT-1](uint)|<next fd>...
 *---+---------------+--------------+-----------------+-----------------+---+--------------------------+------------
 *每个block包含整数个fd，其数量是BLOCK_SIZE / FD_SIZE。fd索引编号从1开始。
 *根目录的长度表示目录下文件个数.
 */
#define FBLK_CNT 3
#define FD_SIZE (sizeof(unsigned char) + sizeof(unsigned int) + sizeof(unsigned int) * FBLK_CNT)
#define MAX_FD ((BLOCK_SIZE / FD_SIZE) * (RESERVED_BLOCK_CNT - 1))

/* dir item
 *+---------------+-------------------+------------+
 *|tag_used(uchar)|file_name[32](char)|fd_idx(uint)|
 *+---------------+-------------------+------------+
 */
#define DIR_ITEM_SIZE (sizeof(unsigned char) + 32 * sizeof(char) + sizeof(unsigned int))
//每个block包含dir item的最大数目
#define DIR_ITEM_CNT_PER_BLOCK (BLOCK_SIZE / DIR_ITEM_SIZE)

//文件缓冲大小
#define FBUF_SIZE BLOCK_SIZE
struct sf_file_t {
#define MAX_FILE_LEN (BLOCK_SIZE * FBLK_CNT)
    unsigned int   file_len;
    unsigned char *buf;
    unsigned long  offset;
    unsigned int   fd_idx;
    unsigned int   cur_blk;
    unsigned char  type;
};

//打开文件最大数
#define SF_MAX_OPEN 32
struct sf_file_t open_file_table[SF_MAX_OPEN];
static unsigned open_file_cnt;

//读取一个block的缓存.
static unsigned char blkbuf[BLOCK_SIZE];

/* --- fd --- */
static unsigned char fd_blk_buf[BLOCK_SIZE];
#define FD_BLK_BUF_BEGIN ((unsigned char *)fd_blk_buf)
static unsigned loaded_blk;
#define FD_IDX_TO_BLK_NUM(fd_idx) \
    ((((fd_idx) - 1) / fdcnt_per_blk) + 1)
#define FD_IDX_TO_BLK_OFFSET(fd_idx) \
    ((((fd_idx) - 1) % fdcnt_per_blk) * FD_SIZE)
static unsigned char *sf_get_fd(unsigned fd_idx)
{
    static const unsigned fdcnt_per_blk = BLOCK_SIZE / FD_SIZE;
    static unsigned char *fd;
    unsigned blk = FD_IDX_TO_BLK_NUM(fd_idx);
    unsigned long offset = FD_IDX_TO_BLK_OFFSET(fd_idx);

    if (blk != loaded_blk) {
        write_block(loaded_blk, FD_BLK_BUF_BEGIN);
        read_block(blk, FD_BLK_BUF_BEGIN);
        loaded_blk = blk;
    }
    fd = FD_BLK_BUF_BEGIN + offset;
    return fd;
}
static void sf_save_fd_blk()
{
    if (loaded_blk != 0)
        write_block(loaded_blk, FD_BLK_BUF_BEGIN);
}
static unsigned sf_find_free_fd()
{
    unsigned fd_idx;
    unsigned char *fd;
    for (fd_idx = 2; fd_idx <= MAX_FD; ++fd_idx) {
        fd = sf_get_fd(fd_idx);
        if (*fd == 0) {
            return fd_idx;
        }
    }
    return 0;
}
#undef FD_IDX_TO_BLK_NUM
#undef FD_IDX_TO_BLK_OFFSET
#undef FD_BLK_BUF_BEGIN

#define FD_TAG_USED(fd) *((unsigned char *)(fd))
#define FD_LEN(fd)      *((unsigned int *)(((unsigned char *)(fd)) + 1))
#define FD_BLK(fd, pos) *((unsigned int *)(((unsigned char *)(fd)) + 1) + pos + 1)
static void sf_fd_set_used(unsigned fd_idx)
{
    FD_TAG_USED(sf_get_fd(fd_idx)) = 1;
}
static void sf_fd_set_free(unsigned fd_idx)
{
    FD_TAG_USED(sf_get_fd(fd_idx)) = 0;
}
//this fn is not used.
//static int sf_fd_is_used(unsigned fd_idx)
//{
//    return FD_TAG_USED(sf_get_fd(fd_idx)) == 1;
//}
static void sf_fd_set_len(unsigned fd_idx, unsigned len)
{
    FD_LEN(sf_get_fd(fd_idx)) = len;
}
static unsigned sf_fd_len(unsigned fd_idx)
{
    return FD_LEN(sf_get_fd(fd_idx));
}
static unsigned sf_fd_blk(unsigned fd_idx, unsigned pos)
{
    return FD_BLK(sf_get_fd(fd_idx), pos);
}
static void sf_fd_set_blk(unsigned fd_idx, unsigned pos, unsigned blk)
{
    FD_BLK(sf_get_fd(fd_idx), pos) = blk;
}

#undef FD_TAG_USED
#undef FD_LEN
#undef FD_BLK
/* --- bitmap --- */
static unsigned char bitmap[BLOCK_SIZE];
static void sf_load_bitmap()
{
    read_block(0, bitmap);
}
static void sf_save_bitmap()
{
    write_block(0, bitmap);
}
static inline unsigned sf_read_bitmap(unsigned n)
{
    return bitmap[n >> 3] >> (7 - (n & (8 - 1))) & 1;
}
static inline void sf_bitmap_write_0(unsigned n)
{
    bitmap[n >> 3] &= ~(1 << (7 - (n & (8 - 1))));
}
static void sf_bitmap_write_1(unsigned n)
{
    bitmap[n >> 3] |= 1 << (7 - (n & (8 - 1)));
}
/* --- mkfs.sf --- */
int sf_mkfs(const char *file)
{
    static unsigned char blkbuf[BLOCK_SIZE];
    unsigned i;
    memset(blkbuf, 0x00, BLOCK_SIZE);
    for (i = 0; i < BLOCK_COUNT; ++i) {
        write_block(i, blkbuf);
    }
    if (file != NULL)
        if (save_to_file(file) != 0) return E_SF_MKFS_FAILED;
    return 0;
}
/* --- load & store --- */
int sf_load(const char *file)
{
    if (load_from_file(file) != 0) return E_SF_LOAD_FAILED;
    sf_load_bitmap();
    memset(open_file_table, 0x00, sizeof(struct sf_file_t) * SF_MAX_OPEN);
    loaded_blk = 0;
    return 0;
}
int sf_store(const char *file)
{
    sf_save_bitmap();
    sf_save_fd_blk();
    if (save_to_file(file) != 0) return E_SF_STORE_FAILED;
    return 0;
}
/* --- block --- */
static unsigned sf_find_free_block()
{
    unsigned i;
    for (i = RESERVED_BLOCK_CNT; i < BLOCK_COUNT; ++i) {
        if (sf_read_bitmap(i) == 0) {
            return i;
        }
    }
    return 0;
}
/* --- dir item --- */
#define DIR_ITEM_TAG_USED(di) (*((unsigned char *)(di)))
#define DIR_ITEM_FNAME(di)    ((char *)((unsigned char *)(di) + 1))
#define DIR_ITEM_FDIDX(di)    (*((unsigned int *)((unsigned char *)(di) + 33)))
/* --- root dir operations --- */
static int sf_root_add_dir_item(const char *fname, unsigned fd_idx)
{
    unsigned fcnt = sf_fd_len(1), blk;
    unsigned i, j;
    unsigned char *cur_di;
    int found = 0;
    //find free dir item
    i = 0;
    while (!found && i < FBLK_CNT) {
        blk = sf_fd_blk(1, i);
        if (blk != 0) { //no need to assign new block to rootdir
            read_block(blk, blkbuf);
            cur_di = blkbuf;
            //find free dir item in this block
            for (j = 0; j < DIR_ITEM_CNT_PER_BLOCK; ++j) {
                if (DIR_ITEM_TAG_USED(cur_di) == 0) {//found
                    found = 1;
                    break;
                } else {
                    cur_di += DIR_ITEM_SIZE;
                }
            }
        } else { //need to assign new block to rootdir
            blk = sf_find_free_block();
            sf_fd_set_blk(1, i, blk);
            sf_bitmap_write_1(blk);
            read_block(blk, blkbuf);
            cur_di = blkbuf;
            break;
        }
        ++i;
    }
    if (i == FBLK_CNT) {//reached max file count
        return E_NO_SPACE_LEFT;
    }
    DIR_ITEM_TAG_USED(cur_di) = 1;
    strcpy(DIR_ITEM_FNAME(cur_di), fname);
    DIR_ITEM_FDIDX(cur_di) = fd_idx;
    write_block(blk, blkbuf);
    sf_fd_set_len(1, fcnt + 1);
    return 0;
}
static unsigned sf_root_delete_dir_item(const char *fname)
{
    unsigned fcnt = sf_fd_len(1), blk, fd_idx = 0;
    unsigned i, j, k;
    unsigned char *cur_di;
    int found = 0;
    //find dir item
    i = 0;
    k = 0;
    while (!found && i < FBLK_CNT && k < fcnt) {
        blk = sf_fd_blk(1, i);
        if (blk != 0) {
            read_block(blk, blkbuf);
            cur_di = blkbuf;
            for (j = 0; j < DIR_ITEM_CNT_PER_BLOCK; ++j) {
                if (DIR_ITEM_TAG_USED(cur_di) == 1) {
                    if (!strcmp(DIR_ITEM_FNAME(cur_di), fname)) {
                        found = 1;
                        fd_idx = DIR_ITEM_FDIDX(cur_di);
                        break;
                    } else {
                        ++k;
                    }
                }
                cur_di += DIR_ITEM_SIZE;
            }
        } else {
            break;
        }
        ++i;
    }
    if (i == FBLK_CNT) {
        return 0;
    }
    if (k == fcnt) {
        return 0;
    }
    if (blk == 0) {
        return 0;
    }
    DIR_ITEM_TAG_USED(cur_di) = 0;
    write_block(blk, blkbuf);
    fcnt--;
    sf_fd_set_len(1, fcnt);
    return fd_idx;
}
static unsigned sf_root_find_file(const char *fname)
{
    unsigned int file_cnt = sf_fd_len(1);
    unsigned int i, j, k, blk;
    unsigned char *cur_di;
    if (file_cnt == 0) return 0;
    k = 0;
    for (i = 0; i < FBLK_CNT; ++i) {
        blk = sf_fd_blk(1, i);
        if (blk == 0) { //impossible
            return E_UNKNOWN;
        }
        read_block(blk, blkbuf);
        cur_di = blkbuf;
        for (j = 0; j < DIR_ITEM_CNT_PER_BLOCK && k < file_cnt; ++j) {
            if (DIR_ITEM_TAG_USED(cur_di) == 1) {
                if (!strcmp(DIR_ITEM_FNAME(cur_di), fname)) {
                    return DIR_ITEM_FDIDX(cur_di);
                }
                ++k;
            }
            cur_di += DIR_ITEM_SIZE;
        }
        if (k == file_cnt) break;
    }
    return 0;
}
/* --- file operation --- */
int sf_create(const char *file_name)
{
    unsigned i, fd_idx, ret;
    if (sf_root_find_file(file_name) > 0) return E_FILE_ALREADY_EXISTS;
    fd_idx = sf_find_free_fd();
    if (fd_idx == 0) return E_NO_SPACE_LEFT;
    ret = sf_root_add_dir_item(file_name, fd_idx);
    if (ret != 0) return ret;
    sf_fd_set_used(fd_idx);
    sf_fd_set_len(fd_idx, 0);
    for (i = 0; i < FBLK_CNT; ++i) {
        sf_fd_set_blk(fd_idx, i, 0);
    }
    return 0;
}
int sf_destroy(const char *file_name)
{
    unsigned fd_idx, i;
    fd_idx = sf_root_delete_dir_item(file_name);
    if (fd_idx == 0) {
        return E_FILE_NOT_FOUND;
    }
    for (i = 0; i < FBLK_CNT; ++i) {
        if (sf_fd_blk(fd_idx, i) != 0) {
            sf_bitmap_write_0(i);
        }
    }
    sf_fd_set_free(fd_idx);
    return 0;
}
void sf_directory()
{
    unsigned int file_cnt = sf_fd_len(1);
    unsigned int i, j, k, blk;
    unsigned char *cur_di;
    if (file_cnt == 0) return;
    puts("name\tlength");
    k = 0;
    for (i = 0; i < FBLK_CNT; ++i) {
        blk = sf_fd_blk(1, i);
        if (blk == 0) { //impossible
            //err...
        }
        read_block(blk, blkbuf);
        cur_di = blkbuf;
        for (j = 0; j < DIR_ITEM_CNT_PER_BLOCK && k < file_cnt; ++j) {
            if (DIR_ITEM_TAG_USED(cur_di) == 1) {
                printf("%s\t", DIR_ITEM_FNAME(cur_di));
                printf("%u\n", sf_fd_len(DIR_ITEM_FDIDX(cur_di)));
                ++k;
            }
            cur_di += DIR_ITEM_SIZE;
        }
        if (k == file_cnt) break;
    }
}
static unsigned sf_find_free_file()
{
    unsigned i;
    for (i = 0; i < SF_MAX_OPEN; ++i) {
        if (open_file_table[i].fd_idx == 0)
            return i;
    }
    return SF_MAX_OPEN;
}

#define OFFSET_TO_BLK_IDX(offset) \
    ((offset) / BLOCK_SIZE)
#define OFFSET_TO_BLK_OFFSET(offset) \
    ((offset) % BLOCK_SIZE)
sf_file sf_open(const char *file_name)
{
    unsigned fd_idx, oft_idx;
    struct sf_file_t *cur_file;
    fd_idx = sf_root_find_file(file_name);
    if (fd_idx != 0) {
        oft_idx = sf_find_free_file();
        if (oft_idx == SF_MAX_OPEN) return E_MAX_OPEN;
        cur_file = &open_file_table[oft_idx];
        cur_file->fd_idx = fd_idx;
        cur_file->file_len = sf_fd_len(fd_idx);
        cur_file->buf = malloc(FBUF_SIZE);
        cur_file->cur_blk = sf_fd_blk(fd_idx, 0);
        if (cur_file->cur_blk != 0)
            read_block(cur_file->cur_blk, cur_file->buf);
        cur_file->offset = 0;
        open_file_cnt++;
        return oft_idx;
    }
    return E_FILE_NOT_FOUND;
}
int sf_close(sf_file file)
{
    struct sf_file_t *cur_file = &open_file_table[file];
    if (cur_file->fd_idx == 0) {
        return E_FILE_NOT_OPENED;
    }
    if (cur_file->cur_blk != 0) {
        write_block(cur_file->cur_blk, cur_file->buf);
    }
    sf_fd_set_len(cur_file->fd_idx, cur_file->file_len);
    free(cur_file->buf);
    cur_file->fd_idx = 0;
    open_file_cnt--;
    return 0;
}
unsigned sf_read(sf_file file, void *mem_area, unsigned count)
{
    struct sf_file_t *cur_file = &open_file_table[file];
    unsigned blk_idx = OFFSET_TO_BLK_IDX(cur_file->offset), blk_offset = OFFSET_TO_BLK_OFFSET(cur_file->offset);
    unsigned blk = sf_fd_blk(cur_file->fd_idx, blk_idx);
    unsigned read_cnt, ret;
    if (count == 0) return 0;
    if (cur_file->offset >= cur_file->file_len) return 0;
    if (cur_file->offset + count > cur_file->file_len) count = cur_file->file_len - cur_file->offset;
    ret = count;
    //保证操作开始时cur_file->offset位于cur_file->cur_blk中.
    if (cur_file->cur_blk != blk) {
        if (cur_file->cur_blk != 0)
            write_block(cur_file->cur_blk, cur_file->buf);
        read_block(blk, cur_file->buf);
        cur_file->cur_blk = blk;
    }
    read_cnt = (BLOCK_SIZE - blk_offset) < count ? (BLOCK_SIZE - blk_offset) : count;
    memcpy(mem_area, cur_file->buf + blk_offset, read_cnt);
    mem_area += read_cnt;
    cur_file->offset += read_cnt;
    count -= read_cnt;
    while (count > 0) {
        write_block(blk, cur_file->buf);
        blk = sf_fd_blk(cur_file->fd_idx, blk_idx + 1);
        read_block(blk, cur_file->buf);
        cur_file->cur_blk = blk;
        read_cnt = BLOCK_SIZE < count ? BLOCK_SIZE : count;
        memcpy(mem_area, cur_file->buf, read_cnt);
        mem_area += read_cnt;
        cur_file->offset += read_cnt;
        count -= read_cnt;
    }
    return ret;
}
unsigned sf_write(sf_file file, const void *mem_area, unsigned count)
{
    struct sf_file_t *cur_file = &open_file_table[file];
    unsigned blk_idx = OFFSET_TO_BLK_IDX(cur_file->offset), blk_offset = OFFSET_TO_BLK_OFFSET(cur_file->offset);
    unsigned blk;
    unsigned i, new_blk;
    unsigned write_cnt, ret;
    if (count == 0) return 0;
    if (cur_file->offset == MAX_FILE_LEN) return 0;
    if (cur_file->offset + count > MAX_FILE_LEN) {
        count = MAX_FILE_LEN - cur_file->offset;
    }
    //若offset + count 超出当前最大block,则分配足够多的block.
    for (i = 0; i <= OFFSET_TO_BLK_IDX(cur_file->offset + count); ++i) {
        if (sf_fd_blk(cur_file->fd_idx, i) == 0) {
            new_blk = sf_find_free_block();
            if (new_blk == 0) {//空间不足
                count = (i * BLOCK_SIZE) - cur_file->offset;
                break;
            }
            sf_bitmap_write_1(new_blk);
            sf_fd_set_blk(cur_file->fd_idx, i, new_blk);
        }
    }
    ret = count;
    blk = sf_fd_blk(cur_file->fd_idx, blk_idx);
    //保证操作开始时cur_file->offset位于cur_file->cur_blk中.
    if (cur_file->cur_blk != blk) {
        if (cur_file->cur_blk != 0)
            write_block(cur_file->cur_blk, cur_file->buf);
        read_block(blk, cur_file->buf);
        cur_file->cur_blk = blk;
    }
    write_cnt = (BLOCK_SIZE - blk_offset) < count ? (BLOCK_SIZE - blk_offset) : count;
    memcpy(cur_file->buf + blk_offset, mem_area, write_cnt);
    cur_file->offset += write_cnt;
    count -= write_cnt;
    while (count > 0) {
        write_block(blk, cur_file->buf);
        blk = sf_fd_blk(cur_file->fd_idx, blk_idx + 1);
        read_block(blk, cur_file->buf);
        cur_file->cur_blk = blk;
        write_cnt = BLOCK_SIZE < count ? BLOCK_SIZE : count;
        memcpy(cur_file->buf, mem_area, write_cnt);
        cur_file->offset += write_cnt;
        count -= write_cnt;
    }
    if (cur_file->file_len < cur_file->offset) cur_file->file_len = cur_file->offset;
    return ret;
}
void sf_lseek(sf_file file, int len, int whence)
{
    struct sf_file_t *cur_file = &open_file_table[file];
    if (cur_file->fd_idx == 0) {
        return;
    }
    switch (whence) {
    case SF_SEEK_CUR:
        if (len < 0 && (unsigned)(len * (-1)) > cur_file->offset) cur_file->offset = 0;
        else if (cur_file->offset + len > MAX_FILE_LEN) cur_file->offset = MAX_FILE_LEN;
        else cur_file->offset += len;
        break;
    case SF_SEEK_SET:
        if (len < 0) cur_file->offset = 0;
        else if (len > MAX_FILE_LEN) cur_file->offset = MAX_FILE_LEN;
        else cur_file->offset = len;
        break;
    case SF_SEEK_END:
        if (len < 0 && (unsigned)(len * (-1)) > cur_file->file_len) cur_file->offset = 0;
        else if (len + cur_file->file_len > MAX_FILE_LEN) cur_file->offset = MAX_FILE_LEN;
        else cur_file->offset = cur_file->file_len + len;
        break;
    default:
        break;
    }
}
/* --- error --- */
struct sf_errinfo_t {
    int errid;
    const char *errinfo;
};
#define ERR_CNT 9
static struct sf_errinfo_t sf_errinfo_table[ERR_CNT] = {
    {E_FILE_NOT_FOUND, "file not found"},
    {E_FILE_ALREADY_EXISTS, "file already exists"},
    {E_NO_SPACE_LEFT, "no space left on this disk"},
    {E_MAX_OPEN, "cannot open more file"},
    {E_FILE_NOT_OPENED, "file not opened"},
    {E_SF_LOAD_FAILED, "load fs failed"},
    {E_SF_STORE_FAILED, "store fs failed"},
    {E_SF_MKFS_FAILED, "mkfs failed"},
    {E_UNKNOWN, "unknown error"},
};
const char *sf_err(int errid)
{
    unsigned i;
    for (i = 0; i < ERR_CNT; ++i) {
        if (sf_errinfo_table[i].errid == errid) {
            return sf_errinfo_table[i].errinfo;
        }
    }
    return NULL;
}
