#include <stdio.h>
#include <limits.h>
#include "mem_mgr_bf.h"

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

static inline void list_add_bf(void *p, void *before)
{
    void *pos;
    LIST_FOR_EACH_AFTER(FREE_LIST_HEAD, pos, before) {
        if (GET_SIZE_FRONT(pos) >= GET_SIZE_FRONT(p)) break;
    }
    LIST_ADD_BEFORE(p, pos);
}

void mm_init_bf()
{
    SET_ALL_FRONT(MEM_START, MEM_SIZE - HEAD_SIZE - FOOT_SIZE, FREE_LIST_HEAD, TAG_FREE, FREE_LIST_HEAD);
    SET_ALL_FRONT(FREE_LIST_HEAD, 0, MEM_START, TAG_FREE, MEM_START);
}

void *mm_request_bf(int n, int *steps)
{
    void *p, *prev, *next;
    void *best;
    unsigned int best_size;
    unsigned int size;
    void *new_p;
    if (steps != NULL) *steps = 0;
    if (n <= 0) return NULL;
    best = NULL;
    best_size = INT_MAX;
    LIST_FOR_EACH(FREE_LIST_HEAD, p) {
        if (steps != NULL) (*steps)++;
        size = GET_SIZE_FRONT(p);
        if (size >= (unsigned int)n) {
            best_size = size;
            best = p;
            break;
        }
    }
    if (best == NULL) return NULL;
    if (best_size >= (unsigned int)n && best_size < n + HEAD_SIZE + FOOT_SIZE + 1) { //刚好相等 或 多出来的不足以存放(head+foot+1)，就把整块给用户
        LIST_DEL(best);
        SET_TAG_FRONT(best, TAG_USED);
        return GET_USER_START_FRONT(best);
    } else if (best_size >= n + HEAD_SIZE + FOOT_SIZE + 1) { //足够多，可以分成两块，一块给用户，另一块空闲
        prev = GET_PREV_FRONT(best);
        next = GET_NEXT_FRONT(best);
        SET_SIZE_FRONT(best, n);
        SET_TAG_FRONT(best, TAG_USED);
        new_p = GET_BACK_FRONT(best);
        SET_SIZE_FRONT(new_p, best_size - n - FOOT_SIZE - HEAD_SIZE);
        SET_TAG_FRONT(new_p, TAG_FREE);
        SET_NEXT_FRONT(prev, new_p);
        SET_PREV_FRONT(new_p, prev);
        SET_PREV_FRONT(next, new_p);
        SET_NEXT_FRONT(new_p, next);
        LIST_DEL(new_p);
        list_add_bf(new_p, FREE_LIST_HEAD);
        return GET_USER_START_FRONT(best);
    }
    return NULL;
}

void mm_release_bf(void *p)
{
    void *p_left, *p_right, *prev, *next;
    int p_size, pl_size, pr_size;
    if (p == NULL) return;
    p = (char *)p - HEAD_SIZE;
    if (IS_FIRST_FRONT(p) && IS_LAST_FRONT(p)) {
        mm_init_bf();
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
            list_add_bf(p, prev);
            return;
        } else {
            SET_TAG_FRONT(p, TAG_FREE);
            list_add_bf(p, FREE_LIST_HEAD);
            return;
        }
    } else if (IS_LAST_FRONT(p)) {
        p_left = GET_FRONT_BACK(p);
        if (GET_TAG_FRONT(p_left) == TAG_FREE) {
            p_size = GET_SIZE_FRONT(p);
            pl_size = GET_SIZE_FRONT(p_left);
            next = GET_NEXT_FRONT(p_left);
            prev = GET_PREV_FRONT(p_left);
            SET_SIZE_FRONT(p_left, p_size + HEAD_SIZE + FOOT_SIZE + pl_size);
            SET_NEXT_FRONT(p_left, next);
            LIST_DEL(p_left);
            list_add_bf(p_left, prev);
            return;
        } else {
            SET_TAG_FRONT(p, TAG_FREE);
            list_add_bf(p, FREE_LIST_HEAD);
            return;
        }
    } else {
        p_left = GET_FRONT_BACK(p);
        p_right = GET_BACK_FRONT(p);
        if (GET_TAG_FRONT(p_left) == TAG_FREE && GET_TAG_FRONT(p_right) == TAG_USED) {
            p_size = GET_SIZE_FRONT(p);
            pl_size = GET_SIZE_FRONT(p_left);
            next = GET_NEXT_FRONT(p_left);
            prev = GET_PREV_FRONT(p_left);
            SET_SIZE_FRONT(p_left, p_size + HEAD_SIZE + FOOT_SIZE + pl_size);
            SET_NEXT_FRONT(p_left, next);
            LIST_DEL(p_left);
            list_add_bf(p_left, prev);
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
            list_add_bf(p, prev);
            return;
        } else if (GET_TAG_FRONT(p_left) == TAG_FREE && GET_TAG_FRONT(p_right) == TAG_FREE) {
            p_size = GET_SIZE_FRONT(p);
            pr_size = GET_SIZE_FRONT(p_right);
            pl_size = GET_SIZE_FRONT(p_left);
            LIST_DEL(p_right);
            next = GET_NEXT_FRONT(p_left);
            prev = GET_PREV_FRONT(p_left);
            SET_SIZE_FRONT(p_left, pl_size + FOOT_SIZE + HEAD_SIZE + p_size + FOOT_SIZE + HEAD_SIZE + pr_size);
            SET_NEXT_FRONT(p_left, next);
            LIST_DEL(p_left);
            list_add_bf(p_left, prev);
            return;
        } else if (GET_TAG_FRONT(p_left) == TAG_USED && GET_TAG_FRONT(p_right) == TAG_USED) {
            SET_TAG_FRONT(p, TAG_FREE);
            list_add_bf(p, FREE_LIST_HEAD);
            return;
        } else {
            //error
            fprintf(stderr, "ERROR!\n");
        }
    }
    return;
}

double get_ratio_bf()
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
