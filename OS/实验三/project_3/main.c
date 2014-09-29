#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define MAX_RS 2048
#define MAX_MEM 256
static size_t MEM_SIZE = 32;
static const int P = 256;
static int M[MAX_MEM];
static int RS[MAX_RS];

void print_rs()
{
    size_t i;
    puts("RS:");
    for (i = 0; i < MAX_RS; ++i) {
        printf("%d, ", RS[i]);
    }
    putchar('\n');
}

size_t load_page()
{
    size_t i, j, cnt;
    cnt = 0;
    memset(M, 0xFF, MAX_MEM * sizeof(int));
    for (i = 0; i < MAX_RS; i++) {
        for (j = 0; j < cnt; j++) {
            if (RS[i] == M[j]) {
                break;
            }
        }
        if (j == cnt) {
            M[cnt++] = RS[i];
            if (cnt == MEM_SIZE) {
                break;
            }
        }
    }
    return i + 1;
}

int in_mem(int page)
{
    size_t i;
    for (i = 0; i < MEM_SIZE; ++i) {
        if (M[i] == page) return 1;
    }
    return 0;
}

void make_rs(int p, int e, int m, double t)
{
    double r;
    size_t cnt = 0;
    int i;
    //printf("引用串长度:%d, 物理内存尺寸:%d, 虚拟内存尺寸:%d, 工作面起始位置:%d, 工作面包含页面数:%d, 工作面移动率：%d, t:%f\n", MAX_RS, MAX_MEM, P, p, e, m, t);
    srand(time(NULL) + rand() % 10);
    while (cnt < MAX_RS) {
        for (i = 0; i < m; i++)
            RS[cnt++] = p + rand() % (e + 1);
        r = rand() / (double)RAND_MAX;
        if (r < t) p = rand() % (P - e);
        else p = (p + 1) % (P - e);
    }
}

size_t opt_repl_alg()
{
    static int count_max, count_now;
    size_t i, j, k, max, start, repl_cnt = 0;
    start = load_page();
    for (i = start; i < MAX_RS; ++i) {
        if (!in_mem(RS[i])) {
            repl_cnt++;
            max = count_max = 0;
            for (j = 0; j < MEM_SIZE; ++j) {
                count_now = 0;
                for (k = i + 1; k < MAX_RS; ++k) {
                    count_now++;
                    if (M[j] == RS[k]) {
                        break;
                    }
                }
                if (count_max < count_now) {
                    max = j;
                    count_max = count_now;
                }
            }
            M[max] = RS[i];
        }
    }
    return repl_cnt;
}

size_t rand_repl_alg()
{
    size_t i, start, x, repl_cnt = 0;
    start = load_page();
    for (i = start; i < MAX_RS; ++i) {
        if (!in_mem(RS[i])) {
            repl_cnt++;
            x = rand() % MEM_SIZE;
            M[x] = RS[i];
        }
    }
    return repl_cnt;
}

size_t FIFO_repl_alg()
{
    size_t oldest = 0;
    size_t i, start, repl_cnt = 0;
    start = load_page();
    for (i = start; i < MAX_RS; ++i) {
        if (!in_mem(RS[i])) {
            repl_cnt++;
            M[oldest] = RS[i];
            oldest++;
            oldest %= MEM_SIZE;
        }
    }
    return repl_cnt;
}

size_t LRU_repl_alg()
{
    static int queue[MAX_MEM + 1];
    size_t i, j, k, start, repl_cnt = 0, front = 0;
    start = load_page();
    memset(queue, 0xFF, (MEM_SIZE + 1) * sizeof(int));
    for (i = 0; i < start; ++i) {
        for (j = 0; j < front; ++j) {
            if (queue[j] == RS[i]) {
                queue[front] = RS[i];
                memmove(queue + j, queue + j + 1, (front - j) * sizeof(int));
                break;
            }
        }
        if (j == front) {
            queue[front++] = RS[i];
            assert(front <= MEM_SIZE);
        }
    }
    for (;i < MAX_RS; ++i) {
        for (j = 0; j < front; ++j) {
            if (queue[j] == RS[i]) {
                queue[front] = RS[i];
                memmove(queue + j, queue + j + 1, (front - j) * sizeof(int));
                break;
            }
        }
        if (j == front) {
            repl_cnt++;
            for (k = 0; k < MEM_SIZE; ++k) {
                if (M[k] == queue[0]) {
                    M[k] = RS[i];
                    break;
                }
            }
            queue[front] = RS[i];
            memmove(queue, queue + 1, (front - 0) * sizeof(int));
        }
    }
    return repl_cnt;
}

size_t clock_repl_alg()
{
    static char clock_flags[MAX_MEM];
    size_t pos = 0;
    size_t i, j, start, repl_cnt = 0;
    start = load_page();
    memset(clock_flags, 1, MEM_SIZE);
    for (i = start; i < MAX_RS; ++i) {
        if (!in_mem(RS[i])) {
            repl_cnt++;
            while (1) {
                if (clock_flags[pos] == 0) {
                    M[pos] = RS[i];
                    clock_flags[pos] = 1;
                    pos++;
                    pos %= MEM_SIZE;
                    break;
                } else if (clock_flags[pos] == 1) {
                    clock_flags[pos] = 0;
                    pos++;
                    pos %= MEM_SIZE;
                } else {
                    //error
                    fprintf(stderr, "err!\n");
                }
            }
        } else {
            for (j = 0; j < MEM_SIZE; ++j) {
                if (M[j] == RS[i]) {
                    clock_flags[j] = 1;
                    break;
                }
            }
        }
    }
    return repl_cnt;
}

struct repl_alg_t {
    char *name;
    size_t (*func)(void);
};

static struct repl_alg_t repl_algs[5] = {
    {"OPT", opt_repl_alg},
    {"LRU", LRU_repl_alg},
    {"CLOCK", clock_repl_alg},
    {"FIFO", FIFO_repl_alg},
    {"RAND", rand_repl_alg}
};

void test()
{
    size_t i, j, k, e, repl_cnt[5] = {0};
    float e_factor[4] = {0.2, 0.4, 0.8, 1.6};
    for (k = 0; k < 4; ++k) {
        for (MEM_SIZE = 16; MEM_SIZE <= MAX_MEM; MEM_SIZE += 32) {
            e = ((int)(MEM_SIZE * e_factor[k])) % P;
            fprintf(stderr, "MEM_SIZE=%d, e = %d\n", MEM_SIZE, e);
            memset(repl_cnt, 0x00, 5 * sizeof(int));
            for (i = 0; i < 10000; i++) {
                make_rs(98, e, e * 0.8, 0.2d);
                for (j = 0; j < 5; j++) {
                    repl_cnt[j] += repl_algs[j].func();
                }
            }
            for (j =0; j < 5; j++) {
                fprintf(stderr, "%s\t%d\n", repl_algs[j].name, repl_cnt[j] / 10000);
            }
        }
    }
}

int main(void)
{
    test();
    return 0;
}
