#ifndef MEM_MGR_BF_H
#define MEM_MGR_BF_H

#include "mem_mgr_def.h"

void mm_init_bf();
void *mm_request_bf(int n, int *steps);
void mm_release_bf(void *p);
double get_ratio_bf();

#endif // MEM_MGR_BF_H
