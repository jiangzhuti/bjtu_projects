#ifndef MEM_MGR_FF_H
#define MEM_MGR_FF_H

#include "mem_mgr_def.h"

void mm_init_ff();
void *mm_request_ff(int n, int *steps);
void mm_release_ff(void *p);
double get_ratio_ff();

#endif // MEM_MGR_FF_H
