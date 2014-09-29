#ifndef _DL_COMMON_DL_COMMON_H
#define _DL_COMMON_DL_COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define IN
#define OUT

#define CHECK_MALLOC(ptr) do { \
        if(NULL == ptr) { \
                fprintf(stderr,"LINE %d:内存分配失败!\n",__LINE__); \
                exit(EXIT_FAILURE); \
        } \
}while (0)

#endif
