#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <mem_mgr_ff.h>
#include <mem_mgr_bf.h>
#include <mem_mgr_wf.h>

struct mem_mgr {
    char *name;
    void *(*mm_request)(int, int *);
    void (*mm_release)(void *);
    double (*get_ratio)();
};
static struct mem_mgr mem_mgr_ff = {
    "first_fit",
    mm_request_ff,
    mm_release_ff,
    get_ratio_ff
};
static struct mem_mgr mem_mgr_bf = {
    "best_fit",
    mm_request_bf,
    mm_release_bf,
    get_ratio_bf
};
static struct mem_mgr mem_mgr_wf = {
    "worst_fit",
    mm_request_wf,
    mm_release_wf,
    get_ratio_wf
};
double randn(double mean /* 均值 */, double std /* 标准差 */)
{
    static double V1, V2, S;
    static int use_last = 0;
    double X;

    if (use_last == 0) {
        do {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);
        X = V1 * sqrt(-2 * log(S) / S);
    } else {
        X = V2 * sqrt(-2 * log(S) / S);
    }
    use_last = 1 - use_last;
    return X * std + mean;
}

void test(int sim_steps, struct mem_mgr *mmgr)
{
    int i, n, k;
    double ratio;
    double total_ratio = 0;
    void *ptrs[1024];
    void *p;
    int tmp;
    unsigned long total_size = 0, total_cnt = 0;
    unsigned long total_search_steps = 0;
    int search_steps;
    k = 0;
    for (i = 0; i < sim_steps; i++) {
        do {
            do {
                n = randn(128, 42);
            } while (n <= 0 || (unsigned)n >= MEM_SIZE - HEAD_SIZE - FOOT_SIZE);
            p = mmgr->mm_request(n, &search_steps);
            if (p != NULL) {
                ptrs[k] = p;
                k++;
                total_size += n;
                total_search_steps += search_steps;
                total_cnt++;
            }
        } while (p != NULL);
        ratio = mmgr->get_ratio();
        total_ratio += ratio;
        if (k > 0) {
            tmp = rand() % k;
            mmgr->mm_release(ptrs[tmp]);
            k--;
            if (tmp < k) memmove(ptrs + tmp, ptrs + tmp + 1, (k - tmp)*(sizeof(void *)));
        }
    }
    printf("%s\tavg_ratio = %f, avg_size = %f, avg_search_steps = %f, total_cnt = %lu\n", mmgr->name, total_ratio / sim_steps, (double)total_size / total_cnt, (double)total_search_steps / total_cnt, total_cnt);
}

int main(void)
{
    int steps = 2000000;
    srand(time(0));
    mm_init_ff();
    mm_init_bf();
    mm_init_wf();
    test(steps, &mem_mgr_ff);
    test(steps, &mem_mgr_bf);
    test(steps, &mem_mgr_wf);
    return 0;
}
