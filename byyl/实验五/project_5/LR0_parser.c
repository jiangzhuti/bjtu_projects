#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "LR0_parser.h"
#include "rbtree/rbtree_rc.h"

struct V_t {
    char name;
    enum V_type {
        TYPE_VN, TYPE_VT
    } type;
};

struct P_t {
    struct V_t *Vn;
    struct V_t *V_list[16];
    int n;
};

struct grammar_t {
    struct V_t *start_Vn;
    struct rb_root Vn_set;
    struct V_t *Vn_table[32];
    int Vn_cnt;
    struct rb_root Vt_set;
    struct V_t *Vt_table[32];
    int Vt_cnt;
    struct rb_root P_set;
    struct P_t *P_table[64];
    int P_cnt;
};

//item type
struct item_t {
    struct P_t *p;
    int pos;
};
/* functions */

static inline int cmp_V_t(const void *v1, const void *v2)
{
    return ((struct V_t *)v1)->name - ((struct V_t *)v2)->name;
}
static inline int cmp_P_t(const void *p1, const void *p2)
{
    int result;
    int i;
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

static inline void print_V(const struct container *cont)
{
    printf("[%c]", CONT_DATA(cont, struct V_t)->name);
}
static inline void print_P(const struct container *cont)
{
    int i;
    printf("[%c]->", CONT_DATA(cont, struct P_t)->Vn->name);
    for (i = 0; i < CONT_DATA(cont, struct P_t)->n; ++i) {
        printf("[%c]", CONT_DATA(cont, struct P_t)->V_list[i]->name);
    }
    putchar('\n');
}
static inline void print_P_ptr(const struct P_t *p)
{
    int i;
    printf("[%c]->", p->Vn->name);
    for (i = 0; i < p->n; ++i) {
        printf("[%c]", p->V_list[i]->name);
    }
    putchar('\n');
}

static inline void print_item(const struct container *cont)
{
    int i;
    printf("[%c]->", CONT_DATA(cont, struct item_t)->p->Vn->name);
    for (i = 0; i < CONT_DATA(cont, struct item_t)->pos; ++i) {
        printf("[%c]", CONT_DATA(cont, struct item_t)->p->V_list[i]->name);
    }
    printf("[.]");
    for (; i < CONT_DATA(cont, struct item_t)->p->n; ++i) {
        printf("[%c]", CONT_DATA(cont, struct item_t)->p->V_list[i]->name);
    }
    putchar('\n');
}
static inline void print_item_ptr(const struct container *cont)
{
    int i;
    printf("[%c]->", (*(CONT_DATA(cont, struct item_t*)))->p->Vn->name);
    for (i = 0; i < (*(CONT_DATA(cont, struct item_t*)))->p->n; ++i) {
        printf("[%c]", (*(CONT_DATA(cont, struct item_t*)))->p->V_list[i]->name);
    }
    printf(" %d\n", (*(CONT_DATA(cont, struct item_t*)))->pos);
}

static struct grammar_t G;
static struct rb_root all_items;
static struct item_t *start_item;
static inline struct V_t *find_Vn(char name)
{
    struct V_t key = {.name = name};
    struct container *result;
    result = container_search(&G.Vn_set, &key, cmp_V_t);
    if (result) return (struct V_t *)(result->data);
    return NULL;
}

static inline struct V_t *find_Vt(char name)
{
    struct V_t key = {.name = name};
    struct container *result;
    result = container_search(&G.Vt_set, &key, cmp_V_t);
    if (result) return (struct V_t *)(result->data);
    return NULL;
}

static inline struct V_t *find_V(char name)
{
    struct V_t *result;
    result = find_Vn(name);
    if (result == NULL)
        result = find_Vt(name);
    return result;
}
static inline struct item_t *find_item(struct P_t *p, int pos)
{
    struct item_t key = {.p = p, .pos = pos};
    struct container *result;
    result = container_search(&all_items, &key, cmp_item_t);
    if (result) return (struct item_t *)(result->data);
    return NULL;
}
static inline int Vn_idx(char name)
{
    int i;
    for (i = 0; i < G.Vn_cnt; ++i) {
        if (G.Vn_table[i]->name == name)
            return i;
    }
    return -1;
}
static inline int Vt_idx(char name)
{
    int i;
    for (i = 0; i < G.Vt_cnt + 1; ++i) {
        if (G.Vt_table[i]->name == name)
            return i;
    }
    return -1;
}

static void init_grammar()
{
    G.start_Vn = NULL;
    G.Vn_set = G.Vt_set = G.P_set = RB_ROOT;
    G.P_cnt = G.Vt_cnt = G.Vn_cnt = 0;
}
int input_grammar(FILE *fp)
{
    static char line[1024];
    struct container *cont;
    struct rb_node *n_itr;
    int i;
    init_grammar();
    //read Vn_set & Vt_set
    //skip blank
    while (isspace(fgetc(fp)));
    fseek(fp, -1, SEEK_CUR);
    fgets(line, 1024, fp);
    i = 0;
    while (line[i] != '\n' && line[i] != '\0') {
        if (isspace(line[i])) {
            i++;
            continue;
        }
        ALLOC_CONT(cont, struct V_t);
        CONT_DATA(cont, struct V_t)->type = TYPE_VN;
        CONT_DATA(cont, struct V_t)->name = line[i];
        if (-1 == container_insert(&G.Vn_set, cont, cmp_V_t)) free(cont);
        else G.Vn_cnt++;
        i++;
    }
    while (isspace(fgetc(fp)));
    fseek(fp, -1, SEEK_CUR);
    fgets(line, 1024, fp);
    i = 0;
    while (line[i] != '\n' && line[i] != '\0') {
        if (isspace(line[i])) {
            i++;
            continue;
        }
        ALLOC_CONT(cont, struct V_t);
        CONT_DATA(cont, struct V_t)->type = TYPE_VT;
        CONT_DATA(cont, struct V_t)->name = line[i];
        if (-1 == container_insert(&G.Vt_set, cont, cmp_V_t)) free(cont);
        else G.Vt_cnt++;
        i++;
    }
    while (isspace(fgetc(fp)));
    fseek(fp, -1, SEEK_CUR);
    fgets(line, 1024, fp);
    G.start_Vn = find_Vn(line[0]);
    assert(G.start_Vn != NULL);
    //read productions
    while (isspace(fgetc(fp)));
    fseek(fp, -1, SEEK_CUR);
    fgets(line, 1024, fp);
    while (!feof(fp)) {
        ALLOC_CONT(cont, struct P_t);
        CONT_DATA(cont, struct P_t)->n = 0;
        CONT_DATA(cont, struct P_t)->Vn = find_Vn(line[0]);
        assert(CONT_DATA(cont, struct P_t)->Vn != NULL);
        i = 1;
        while (line[i] != '\n' && line[i] != '\0') {
            if (isspace(line[i])) {
                i++;
                continue;
            }
            CONT_DATA(cont, struct P_t)->V_list[CONT_DATA(cont, struct P_t)->n++] = find_V(line[i]);
            assert(CONT_DATA(cont, struct P_t)->V_list[CONT_DATA(cont, struct P_t)->n - 1] != NULL);
            i++;
        }
        if (-1 == container_insert(&G.P_set, cont, cmp_P_t)) free(cont);
        else G.P_cnt++;
        fgets(line, 1024, fp);
    }
    for (n_itr = rb_first(&G.Vn_set), i = 0; n_itr; n_itr = rb_next(n_itr)) {
        G.Vn_table[i++] = (struct V_t *)(CONT_ENTRY(n_itr)->data);
    }
    assert(i == G.Vn_cnt);
    for (n_itr = rb_first(&G.Vt_set), i = 0; n_itr; n_itr = rb_next(n_itr)) {
        G.Vt_table[i++] = (struct V_t *)(CONT_ENTRY(n_itr)->data);
    }
    assert(i == G.Vt_cnt);
    ALLOC_CONT(cont, struct V_t);
    CONT_DATA(cont, struct V_t)->name = '#';
    CONT_DATA(cont, struct V_t)->type = TYPE_VT;
//    if (-1 == container_insert(&G.Vt_set, cont, cmp_V_t)) free(cont);
    G.Vt_table[G.Vt_cnt] = (struct V_t*)cont->data;
    for (n_itr = rb_first(&G.P_set), i = 0; n_itr; n_itr = rb_next(n_itr)) {
        G.P_table[i++] = (struct P_t *)(CONT_ENTRY(n_itr)->data);
    }
    assert(i == G.P_cnt);
    return 0;
}

void print_grammar()
{
    printf("start:%c\n", G.start_Vn->name);
    printf("Vn:");
    container_traverse(&G.Vn_set, print_V);
    putchar('\n');
    printf("Vt:");
    container_traverse(&G.Vt_set, print_V);
    putchar('\n');
    printf("P:\n");
    container_traverse(&G.P_set, print_P);
}

int make_all_items()
{
    //enhanced grammar
    struct container *cont;
    struct V_t *new_s, *old_s;
    int i, j;
    ALLOC_CONT(cont, struct V_t);
    CONT_DATA(cont, struct V_t)->name = 'Q';
    CONT_DATA(cont, struct V_t)->type = TYPE_VN;
    new_s = (struct V_t *)(cont->data);
    if (-1 == container_insert(&G.Vn_set, cont, cmp_V_t)) free(cont);
    G.Vn_table[G.Vn_cnt++] = new_s;
    ALLOC_CONT(cont, struct P_t);
    CONT_DATA(cont, struct P_t)->Vn = new_s;
    old_s = G.start_Vn;
    CONT_DATA(cont, struct P_t)->n = 1;
    CONT_DATA(cont, struct P_t)->V_list[0] = old_s;
    G.start_Vn = new_s;
    if (-1 == container_insert(&G.P_set, cont, cmp_P_t)) free(cont);
    G.P_table[G.P_cnt++] = (struct P_t *)cont->data;
    //make item set
    for (i = 0; i < G.P_cnt; ++i) {
        for (j = 0; j <= CONT_DATA(cont, struct item_t)->p->n; ++j) {
            ALLOC_CONT(cont, struct item_t);
            CONT_DATA(cont, struct item_t)->p = G.P_table[i];
            assert(G.P_table[i] != NULL);
            CONT_DATA(cont, struct item_t)->pos = j;
            if (-1 == container_insert(&all_items, cont, cmp_item_t)) free(cont);
            else if (CONT_DATA(cont, struct item_t)->p->Vn->name == 'Q' &&
                     CONT_DATA(cont, struct item_t)->pos == 0) {
                start_item = (struct item_t *)(cont->data);
            }
        }
    }
    return 0;
}

void print_items()
{
    container_traverse(&all_items, print_item);
}

static void make_item_set_closure(struct rb_root *item_set)
{
    int flag;
    struct rb_node *node1, *node2;
    struct item_t *item1, *item2;
    struct container *cont;
    do {
        for (node1 = rb_first(item_set); node1;) {
            flag = 0;
            item1 = *((struct item_t **)(CONT_ENTRY(node1)->data));
            if (item1->p->n > item1->pos && item1->p->V_list[item1->pos]->type == TYPE_VN) {
                for (node2 = rb_first(&all_items); node2; node2 = rb_next(node2)) {
                    item2  = (struct item_t *)(CONT_ENTRY(node2)->data);
                    if (item2->p->Vn == item1->p->V_list[item1->pos] && item2->pos == 0) {
                        ALLOC_CONT(cont, struct item_t*);
                        *(CONT_DATA(cont, struct item_t*)) = item2;
                        if (-1 == container_insert(item_set, cont, cmp_ptr)) {
                            free(cont);
                        } else {
                            flag = 1;
                        }
                    }
                }
                if (flag == 1) {
                    node1 = rb_first(item_set);
                    continue;
                }
            }
            node1 = rb_next(node1);
        }
    } while (flag == 1);
}

static void print_item_set(struct rb_root *item_set)
{
    container_traverse(item_set, print_item_ptr);
}

struct LR0_DFA_t {
    struct V_t     *v[32];
    struct state_t {
        struct rb_root itemset;
        int            to_state[32];
    } states[32];
    int state_count;
};

static struct LR0_DFA_t DFA;

static void make_state_item_set(const struct rb_root *itemset, const struct V_t *v, struct rb_root *new_itemset)
{
    struct container *cont;
    struct item_t *item1, *item2;
    *new_itemset = RB_ROOT;
    struct rb_node *node;
    for (node = rb_first(itemset); node; node = rb_next(node)) {
        item1 = *((struct item_t **)(CONT_ENTRY(node)->data));
        if (item1->p->V_list[item1->pos] == v) {
            item2 = find_item(item1->p, item1->pos + 1);
            assert(item2 != NULL);
            ALLOC_CONT(cont, struct item_t*);
            *(CONT_DATA(cont, struct item_t*)) = item2;
            if (-1 == container_insert(new_itemset, cont, cmp_ptr)) free(cont);
        }
    }
    if (new_itemset->rb_node != NULL)
        make_item_set_closure(new_itemset);
}

static inline int find_same_state(struct rb_root *itemset)
{
    int i;
    for (i = 0; i < DFA.state_count; ++i) {
        if (!rb_cmp(&DFA.states[i].itemset, itemset, cmp_ptr)) {
            return i;
        }
    }
    return -1;
}

void make_DFA()
{
    struct rb_root tmp_itemset = RB_ROOT;
    struct container *cont;
    int to_state;
    int i, j;
    i = j = 0;
    for (i = 0; i < G.Vn_cnt; ++i) {
        DFA.v[j] = G.Vn_table[i];
        j++;
    }
    for (i = 0; i < G.Vt_cnt; ++i) {
        DFA.v[j] = G.Vt_table[i];
        j++;
    }
    i = j = 0;

    DFA.state_count = 0;
    DFA.states[0].itemset = RB_ROOT;
    ALLOC_CONT(cont, struct item_t*);
    *(CONT_DATA(cont, struct item_t*)) = start_item;
    container_insert(&DFA.states[0].itemset, cont, cmp_ptr);
    make_item_set_closure(&DFA.states[0].itemset);
    DFA.state_count = 1;
    i = j = 0;
    for (i = 0; i < DFA.state_count; ++i) {
        for (j = 0; j < G.Vn_cnt + G.Vt_cnt; ++j) {
            make_state_item_set(&DFA.states[i].itemset, DFA.v[j], &tmp_itemset);
            if (tmp_itemset.rb_node == NULL) {
                DFA.states[i].to_state[j] = -1;
            } else {
                to_state = find_same_state(&tmp_itemset);
                if (to_state == -1) {
                    DFA.states[DFA.state_count].itemset = tmp_itemset;
                    tmp_itemset = RB_ROOT;
                    DFA.states[i].to_state[j] = DFA.state_count;
                    DFA.state_count++;
                } else {
                    DFA.states[i].to_state[j] = to_state;
                }
            }
        }
    }
}

static int state_go(unsigned s_from, const struct V_t *v)
{
    int i;
    for (i = 0; i < G.Vn_cnt + G.Vt_cnt; ++i) {
        if (DFA.v[i] == v) {
            break;
        }
    }
    assert(i != G.Vn_cnt + G.Vt_cnt);
    if (DFA.states[s_from].to_state[i] != -1) {
        return DFA.states[s_from].to_state[i];
    }
    return -1;
}

void print_DFA()
{
    int i, j;
    for (i = 0; i < DFA.state_count; ++i) {
        printf("i%d:\n", i);
        print_item_set(&DFA.states[i].itemset);
    }
    printf("DFA:\n");
    for (i = 0; i < DFA.state_count; ++i) {
        for (j = 0; j < G.Vn_cnt + G.Vt_cnt; ++j) {
            if (DFA.states[i].to_state[j] == -1) continue;
            printf("i%d =[%c]=> i%d\n", i, DFA.v[j]->name, DFA.states[i].to_state[j]);
        }
    }
}

struct action_t {
    enum act_type_t {
        SHIFTIN,
        REDUCE,
        ACCEPT,
        ERROR
    } act_type;
    int idx;
};
static struct action_t action_table[32][32];
static int goto_table[32][32];

void make_parsing_table()
{
    struct item_t *item;
    struct V_t *VX;
    int i, j, k;
    int state_j;
    for (i = 0; i < DFA.state_count; ++i) {
        for (j = 0; j < G.Vn_cnt + G.Vt_cnt + 1/*1 for '#' */; ++j) {
            action_table[i][j].act_type = ERROR;
            goto_table[i][j] = -1;
        }
    }
    for (i = 0; i < DFA.state_count; ++i) {
        for (struct rb_node *node = rb_first(&DFA.states[i].itemset); node; node = rb_next(node)) {
            item = *CONT_DATA(CONT_ENTRY(node), struct item_t*);
            if (item->pos == item->p->n) {
                if (item->p->Vn == G.start_Vn) { //接受项目
                    action_table[i][G.Vt_cnt].act_type = ACCEPT;
                } else { //规约项目
                    for (j = 0; j < G.P_cnt; ++j)
                        if (G.P_table[j] == item->p)
                            break;
                    for (k = 0; k < G.Vt_cnt + 1; ++k) {
                        action_table[i][k].act_type = REDUCE;
                        action_table[i][k].idx = j;
                    }
                }
                continue;
            }
            VX = item->p->V_list[item->pos];
            state_j = state_go(i, VX);
            if (VX->type == TYPE_VT) { //移进项目
                if (state_j != -1) {
                    for (j = 0; j < G.Vt_cnt + 1; ++j) {
                        if (G.Vt_table[j] == VX) {
                            action_table[i][j].act_type = SHIFTIN;
                            action_table[i][j].idx = state_j;
                            break;
                        }
                    }
                }
            } else { //待约项目
                for (j = 0; j < G.Vn_cnt; ++j) {
                    if (G.Vn_table[j] == VX)
                        goto_table[i][j] = state_j;
                }
            }
        }
    }
}

void print_parsing_table()
{
    int i, j;
    printf(" \tACTION");
    for (i = 0; i < G.Vt_cnt + 1; ++i) {
        putchar('\t');
    }
    printf("GOTO\n");
    printf(" \t");
    for (i = 0; i < G.Vt_cnt + 1; ++i)
        printf("%c\t", G.Vt_table[i]->name);
    for (i = 0; i < G.Vn_cnt; ++i)
        printf("%c\t", G.Vn_table[i]->name);
    printf("\n");
    for (i = 0; i < DFA.state_count; ++i) {
        printf("%d\t", i);
        for (j = 0; j < G.Vt_cnt + 1; ++j) {
            if (action_table[i][j].act_type == ERROR) {
                printf("ERR\t");
            } else if (action_table[i][j].act_type == SHIFTIN) {
                printf("s[%d]\t", action_table[i][j].idx);
            } else if (action_table[i][j].act_type == REDUCE) {
                printf("r[%d]\t", action_table[i][j].idx);
            } else if (action_table[i][j].act_type == ACCEPT) {
                printf("ACC\t");
            }
        }
        for (j = 0; j < G.Vn_cnt; ++j) {
            if (goto_table[i][j] != -1) {
                printf("[%d]\t", goto_table[i][j]);
            } else {
                printf(" \t");
            }
        }
        printf("\n");
    }
}

static void LR0_err()
{
    puts("error!");
    exit(EXIT_FAILURE);
}

static void LR0_act(int i)
{(void)i;}

#define STACK_DECL(stack_name, type, size) \
    type stack_name##_stack[size]; \
    unsigned stack_name##_front = 0;

#define PUSH(stack_name, val) \
    do { \
        stack_name##_stack[stack_name##_front] = (val); \
        stack_name##_front++; \
    } while (0)
#define POP(stack_name, n) \
    (stack_name##_front -= (n))
#define PEEK(stack_name) \
    (stack_name##_stack[stack_name##_front - 1])

static void print_state_stack(unsigned state_stack[], unsigned state_front)
{
    unsigned i;
    puts("state stack:");
    for (i = 0; i < state_front; ++i) {
        printf("%d", state_stack[i]);
    }
    putchar('\n');
}

static void print_sym_stack(char sym_stack[], unsigned sym_front)
{
    unsigned i;
    puts("sym stack:");
    for (i = 0; i < sym_front; ++i) {
        printf("%c", sym_stack[i]);
    }
    putchar('\n');
}
#define SHOW_PARSE_DETAIL() \
    do { \
        printf("===== step %d ====\n", steps); \
        print_state_stack(state_stack, state_front); \
        print_sym_stack(sym_stack, sym_front); \
        printf("next sym:\n%c\n", next_sym); \
        printf("action:"); \
        switch (action.act_type) { \
            case ACCEPT:printf("accept\n");break; \
            case ERROR:printf("error\n");break; \
            case SHIFTIN:printf("shiftin, next state = %d\n", action.idx); break; \
            case REDUCE:printf("reduce, with P : ");print_P_ptr(G.P_table[action.idx]);break;\
            default:break;\
        } \
    } while (0)
void LR0_parser(FILE *fp, int show_detail)
{
    STACK_DECL(state, unsigned, 256);
    STACK_DECL(sym, char, 256);
    static char next_sym;
    static struct action_t action;
    static int shift_j, reduce_i;
    static int steps;
    PUSH(state, 0);
    PUSH(sym, '#');
    next_sym = fgetc(fp);
    action = action_table[PEEK(state)][Vt_idx(next_sym)];
    steps = 1;
    if (show_detail) {
        SHOW_PARSE_DETAIL();
    }
    while (action.act_type != ACCEPT) {
        if (action.act_type == ERROR) LR0_err();
        else if (action.act_type == SHIFTIN) {
            shift_j = action.idx;
            PUSH(state, shift_j);
            PUSH(sym, next_sym);
            next_sym = fgetc(fp);
        } else if (action.act_type == REDUCE) {
            reduce_i = action.idx;
            LR0_act(reduce_i);
            POP(state, G.P_table[reduce_i]->n);
            POP(sym, G.P_table[reduce_i]->n);
            PUSH(sym, G.P_table[reduce_i]->Vn->name);
            PUSH(state, goto_table[PEEK(state)][Vn_idx(G.P_table[reduce_i]->Vn->name)]);
        }
        action = action_table[PEEK(state)][Vt_idx(next_sym)];
        steps++;
        if (show_detail) {
            SHOW_PARSE_DETAIL();
        }
    }
    printf("accept!\n");
}

void clean_up()
{
    container_destroy(&G.Vn_set);
    container_destroy(&G.Vt_set);
    container_destroy(&G.P_set);
    container_destroy(&all_items);
    for (int i = 0; i < DFA.state_count; ++i) {
        container_destroy(&DFA.states[i].itemset);
    }
}

#undef STACK_DECL
#undef PUSH
#undef POP
#undef PEEK
#undef SHOW_PARSE_DETAIL
