#include <stdio.h>
#include <limits.h>
#include "mem_mgr_wf.h"

static char mm[MEM_SIZE];
#define MEM_START ((void *)mm)
#define MEM_END   ((void *)(mm + MEM_SIZE))

#define IS_FIRST_FRONT(front) \
    ((void *)(front) == MEM_START)
#define IS_LAST_FRONT(front) \
    ((void *)((char *)(front) + HEAD_SIZE + GET_SIZE_FRONT((front)) + FOOT_SIZE) == MEM_END)
#define IS_FIRST_BACK(back) \
    ((void *)((char *)(back) - FOOT_SIZE - GET_SIZE_BACK((back)) - HEAD_SIZE) == MEM_START)
#define IS_LAST_BACK(back) \
    ((void *)(back) == MEM_END)

static char free_list_head[HEAD_SIZE + FOOT_SIZE];
#define FREE_LIST_HEAD ((void *)free_list_head)

static inline void list_add_wf(void *p, void *after)
{
    void *pos;
    LIST_FOR_EACH_PREV_BEFORE(FREE_LIST_HEAD, pos, after) {
        if (GET_SIZE_FRONT(pos) >= GET_SIZE_FRONT(p)) break;
    }
    LIST_ADD_AFTER(p, pos);
}

void mm_init_wf()
{
    SET_ALL_FRONT(MEM_START, MEM_SIZE - HEAD_SIZE - FOOT_SIZE, FREE_LIST_HEAD, TAG_FREE, FREE_LIST_HEAD);
    SET_ALL_FRONT(FREE_LIST_HEAD, 0, MEM_START, TAG_FREE, MEM_START);
}

void *mm_request_wf(int n, int *steps)
{
    void *prev, *next;
    void *worst;
    unsigned int worst_size;
    void *new_p;
    if (steps != NULL) *steps = 0;
    if (n <= 0) return NULL;
    worst = GET_NEXT_FRONT(FREE_LIST_HEAD);
    worst_size = GET_SIZE_FRONT(worst);
    (*steps)++;
    if (worst == FREE_LIST_HEAD || worst_size < (unsigned int)n) return NULL;
    if (worst_size >= (unsigned int)n && worst_size < n + HEAD_SIZE + FOOT_SIZE + 1) { //刚好相等 或 多出来的不足以存放(head+foot+1)，就把整块给用户
        SET_TAG_FRONT(worst, TAG_USED);
        LIST_DEL(worst);
        return GET_USER_START_FRONT(worst);
    } else if (worst_size >= n + HEAD_SIZE + FOOT_SIZE + 1) { //足够多，可以分成两块，一块给用户，另一块空闲
        prev = GET_PREV_FRONT(worst);
        next = GET_NEXT_FRONT(worst);
        SET_SIZE_FRONT(worst, n);
        SET_TAG_FRONT(worst, TAG_USED);
        new_p = GET_BACK_FRONT(worst);
        SET_SIZE_FRONT(new_p, worst_size - n - FOOT_SIZE - HEAD_SIZE);
        SET_TAG_FRONT(new_p, TAG_FREE);
        SET_NEXT_FRONT(prev, new_p);
        SET_PREV_FRONT(new_p, prev);
        SET_PREV_FRONT(next, new_p);
        SET_NEXT_FRONT(new_p, next);
        LIST_DEL(new_p);
        list_add_wf(new_p, FREE_LIST_HEAD);
        return GET_USER_START_FRONT(worst);
    }
    return NULL;
}

void mm_release_wf(void *p)
{
    void *p_left, *p_right, *prev, *next;
    int p_size, pl_size, pr_size;
    if (p == NULL) return;
    p = (char *)p - HEAD_SIZE;
    if (IS_FIRST_FRONT(p) && IS_LAST_FRONT(p)) {
        mm_init_wf();
        return;
    } else if (IS_FIRST_FRONT(p)) {
        p_right = GET_BACK_FRONT(p);
        if (GET_TAG_FRONT(p_right) == TAG_FREE) {
            p_size = GET_SIZE_FRONT(p);
            pr_size = GET_SIZE_FRONT(p_right);
            prev = GET_PREV_FRONT(p_right);
            next = GET_NEXT_FRONT(p_right);
            SET_SIZE_FRONT(p, p_size + FOOT_SIZE + HEAD_SIZE + pr_size);
            SET_PREV_FRONT(p, prev);
            SET_TAG_FRONT(p, TAG_FREE);
            if (GET_PREV_FRONT(prev) == GET_NEXT_FRONT(prev)) SET_PREV_FRONT(prev, p);
            SET_NEXT_FRONT(prev, p);
            SET_PREV_FRONT(next, p);
            LIST_DEL(p);
            list_add_wf(p, next);
            return;
        } else {
            SET_TAG_FRONT(p, TAG_FREE);
            list_add_wf(p, FREE_LIST_HEAD);
            return;
        }
    } else if (IS_LAST_FRONT(p)) {
        p_left = GET_FRONT_BACK(p);
        if (GET_TAG_FRONT(p_left) == TAG_FREE) {
            p_size = GET_SIZE_FRONT(p);
            pl_size = GET_SIZE_FRONT(p_left);
            next = GET_NEXT_FRONT(p_left);
            SET_SIZE_FRONT(p_left, p_size + HEAD_SIZE + FOOT_SIZE + pl_size);
            SET_NEXT_FRONT(p_left, next);
            LIST_DEL(p_left);
            list_add_wf(p_left, next);
            return;
        } else {
            SET_TAG_FRONT(p, TAG_FREE);
            list_add_wf(p, FREE_LIST_HEAD);
            return;
        }
    } else {
        p_left = GET_FRONT_BACK(p);
        p_right = GET_BACK_FRONT(p);
        if (GET_TAG_FRONT(p_left) == TAG_FREE && GET_TAG_FRONT(p_right) == TAG_USED) {
            p_size = GET_SIZE_FRONT(p);
            pl_size = GET_SIZE_FRONT(p_left);
            next = GET_NEXT_FRONT(p_left);
            SET_SIZE_FRONT(p_left, p_size + HEAD_SIZE + FOOT_SIZE + pl_size);
            SET_NEXT_FRONT(p_left, next);
            LIST_DEL(p_left);
            list_add_wf(p_left, next);
            return;
        } else if (GET_TAG_FRONT(p_left) == TAG_USED && GET_TAG_FRONT(p_right) == TAG_FREE) {
            p_size = GET_SIZE_FRONT(p);
            pr_size = GET_SIZE_FRONT(p_right);
            prev = GET_PREV_FRONT(p_right);
            next = GET_NEXT_FRONT(p_right);
            SET_SIZE_FRONT(p, p_size + FOOT_SIZE + HEAD_SIZE + pr_size);
            SET_PREV_FRONT(p, prev);
            SET_TAG_FRONT(p, TAG_FREE);
            if (GET_PREV_FRONT(prev) == GET_NEXT_FRONT(prev)) SET_PREV_FRONT(prev, p);
            SET_NEXT_FRONT(prev, p);
            SET_PREV_FRONT(next, p);
            LIST_DEL(p);
            list_add_wf(p, next);
            return;
        } else if (GET_TAG_FRONT(p_left) == TAG_FREE && GET_TAG_FRONT(p_right) == TAG_FREE) {
            p_size = GET_SIZE_FRONT(p);
            pr_size = GET_SIZE_FRONT(p_right);
            pl_size = GET_SIZE_FRONT(p_left);
            LIST_DEL(p_right);
            next = GET_NEXT_FRONT(p_left);
            SET_SIZE_FRONT(p_left, pl_size + FOOT_SIZE + HEAD_SIZE + p_size + FOOT_SIZE + HEAD_SIZE + pr_size);
            SET_NEXT_FRONT(p_left, next);
            LIST_DEL(p_left);
            list_add_wf(p_left, next);
            return;
        } else if (GET_TAG_FRONT(p_left) == TAG_USED && GET_TAG_FRONT(p_right) == TAG_USED) {
            SET_TAG_FRONT(p, TAG_FREE);
            list_add_wf(p, FREE_LIST_HEAD);
            return;
        } else {
            //error
            fprintf(stderr, "ERROR!\n");
        }
    }
    return;
}

double get_ratio_wf()
{
    void *p;
    int used_size = 0, total_size = 0;
    p = MEM_START;
    do {
        if (GET_TAG_FRONT(p) == TAG_USED) {
            used_size += GET_SIZE_FRONT(p);
        }
        total_size += GET_SIZE_FRONT(p);
        p = GET_BACK_FRONT(p);
    } while (p != MEM_END);
    return (double)used_size / (double)total_size;
}
