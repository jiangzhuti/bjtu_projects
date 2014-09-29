#ifndef MEM_MGR_WF_H
#define MEM_MGR_WF_H

#include "mem_mgr_def.h"

void mm_init_wf();
void *mm_request_wf(int n, int *steps);
void mm_release_wf(void *p);
double get_ratio_wf();

#endif // MEM_MGR_WF_H
