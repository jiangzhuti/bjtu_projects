#ifndef PROJECT_5_DEF_H
#define PROJECT_5_DEF_H
#include "rbtree/rbtree_rc.h"

#include <stdio.h>

struct V_t {
    char name;
    enum V_type {
        TYPE_VN, TYPE_VT
    } tag;
    struct rb_root FIRSTSET;
    struct rb_root FOLLOWSET;
};

struct P_t {
    struct V_t *Vn;
    struct V_t *V_list[16];
    unsigned n;
};

struct grammar_t {
    struct V_t *start_Vn;
    struct rb_root Vn_set;
    struct V_t *Vn_table[32];
    unsigned Vn_cnt;
    struct rb_root Vt_set;
    struct V_t *Vt_table[32];
    unsigned Vt_cnt;
    struct rb_root P_set;
    struct P_t *P_table[64];
    unsigned P_cnt;
};

//item type
struct item_t {
    struct P_t *p;
    unsigned pos;
};


/* functions */

static inline int cmp_V_t(const void *v1, const void *v2)
{
    return ((struct V_t *)v1)->name - ((struct V_t *)v2)->name;
}

static inline int cmp_P_t(const void *p1, const void *p2)
{
    int result;
    unsigned i;
    result = ((struct P_t *)p1)->n - ((struct P_t *)p2)->n;
    if (result != 0) return result;
    result = (char *)(((struct P_t *)p1)->Vn) - (char *)(((struct P_t *)p2)->Vn);
    if (result != 0) return result;
    for (i = 0; i < ((struct P_t *)p1)->n; ++i) {
        result = (char *)(((struct P_t *)p1)->V_list[i]) - (char *)(((struct P_t *)p2)->V_list[i]);
        if (result != 0) return result;
    }
    return result;
}

static inline int cmp_ptr(const void *ptr1, const void *ptr2)
{
    return *(char **)ptr1 - *(char **)ptr2;
}

static inline int cmp_item_t(const void *i1, const void *i2)
{
    int result;
    result = ((struct item_t *)i1)->p - ((struct item_t *)i2)->p;
    if (result == 0) {
        result = ((struct item_t *)i1)->pos - ((struct item_t *)i2)->pos;
    }
    return result;
}

static inline struct V_t *find_Vn(const struct grammar_t *g, char name)
{
    struct V_t key = {.name = name};
    struct container *result;
    result = container_search(&(g->Vn_set), &key, cmp_V_t);
    if (result) return (struct V_t *)(result->data);
    return NULL;
}

static inline struct V_t *find_Vt(const struct grammar_t *g, char name)
{
    struct V_t key = {.name = name};
    struct container *result;
    result = container_search(&(g->Vt_set), &key, cmp_V_t);
    if (result) return (struct V_t *)(result->data);
    return NULL;
}

static inline struct V_t *find_V(const struct grammar_t *g, char name)
{
    struct V_t *result;
    result = find_Vn(g, name);
    if (result == NULL)
        result = find_Vt(g, name);
    return result;
}

static inline struct item_t *find_item(struct rb_root *items, struct P_t *p, int pos)
{
    struct item_t key = {.p = p, .pos = pos};
    struct container *result;
    result = container_search(items, &key, cmp_item_t);
    if (result) return (struct item_t *)(result->data);
    return NULL;
}

static inline void print_V(const struct container *cont)
{
    printf("[%c]", CONT_DATA(cont, struct V_t)->name);
}

static inline void print_P(const struct container *cont)
{
    unsigned i;
    printf("[%c]->", CONT_DATA(cont, struct P_t)->Vn->name);
    for (i = 0; i < CONT_DATA(cont, struct P_t)->n; ++i) {
        printf("[%c]", CONT_DATA(cont, struct P_t)->V_list[i]->name);
    }
    putchar('\n');
}

static inline void print_item(const struct container *cont)
{
    unsigned i;
    printf("[%c]->", CONT_DATA(cont, struct item_t)->p->Vn->name);
    for (i = 0; i < CONT_DATA(cont, struct item_t)->p->n; ++i) {
        printf("[%c]", CONT_DATA(cont, struct item_t)->p->V_list[i]->name);
    }
    printf(" %d\n", CONT_DATA(cont, struct item_t)->pos);
}

static inline void print_item_ptr(const struct container *cont)
{
    unsigned i;
    printf("[%c]->", (*(CONT_DATA(cont, struct item_t*)))->p->Vn->name);
    for (i = 0; i < (*(CONT_DATA(cont, struct item_t*)))->p->n; ++i) {
        printf("[%c]", (*(CONT_DATA(cont, struct item_t*)))->p->V_list[i]->name);
    }
    printf(" %d\n", (*(CONT_DATA(cont, struct item_t*)))->pos);
}

static void init_grammar(struct grammar_t *g)
{
    g->start_Vn = NULL;
    g->Vn_set = g->Vt_set = g->P_set = RB_ROOT;
    g->P_cnt = g->Vt_cnt = g->Vn_cnt = 0;
}

#endif // PROJECT_5_DEF_H
