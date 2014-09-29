#include <set>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <cstdio>
#include <cstring>

#include "clsdef.h"
#include "project_2_3_def.h"

//全部的符号及其FIRST和FOLLOW集
struct Vset_t Vs[V_COUNT];

//分析表列数
#define M_COL_CNT 8
//分析表行数
#define M_ROW_CNT Vn_COUNT
//分析表的列(Vt U {EOI})
static const int M_cols[M_COL_CNT] =
{T_ID, T_LP, T_RP, T_PLUS, T_SUB, T_MUL, T_DIV, EOI};

struct parsing_table_row_t {
    struct Vset_t *Vns;//指向Vs[]中的某个元素，表示此行的非终结符
    const struct P_t* p[M_COL_CNT];//此行的所有元素，若是产生式则指向P[]中的对应产生式，否则用NULL表示错误
};
//分析表的行
struct parsing_table_row_t M_rows[M_ROW_CNT];

std::queue<int> sym_queue;//符号串(先进先出的队列)
std::stack<int> parsing_stack;//分析栈

/* 工具函数 */

static void check_fopen(FILE *fp, const char *name)
{
    if (fp == NULL) {
        fprintf(stderr, "cannot open file: %s\n", name);
        exit(EXIT_FAILURE);
    }
}

//判断是否在一个set中
static inline bool is_in_set(std::set<int> *s, int x)
{
    return (s->find(x) != s->end());
}
//判断是否在一个vector中
static inline bool is_in_vector(std::vector<int> *v, int x)
{
    return (std::find(v->begin(), v->end(), x) != v->end());
}
//查找Vs[]中符号是参数v的那个元素的下标
static inline int Vs_idx_of(int v)
{
    int i;
    for (i = 0; i < V_COUNT; i++) {
        if (Vs[i].V == v)
            return i;
    }
    return -1;
}
//将一个set中的所有元素加入到另一个set中
static inline void set_add(std::set<int> *to, const std::set<int> *from)
{
    for (std::set<int>::iterator it = from->begin(); it != from->end(); it++) {
        to->insert(*it);
    }
}
//将一个set中除去元素x的所有元素加入到另一个set中
void set_add_except(std::set<int> *to, const std::set<int> *from, int x)
{
    for (std::set<int>::iterator it = from->begin(); it != from->end(); it++) {
        if (*it != x)
            to->insert(*it);
    }
}
//构造Vs[]中所有元素的FIRST集
void make_FIRST()
{
    int old_size, new_size;
    int k;
    bool ctnu;
    std::set<int> *s;
    //for terminals: FIRST set only includes itself
    for (int i = 0; i < Vt_COUNT; i++) {
        Vs[Vn_COUNT + i].FIRST.insert(Vs[Vn_COUNT + i].V);
    }
    //for nonterminals
    do {
        old_size = 0;
        new_size = 0;
        for (int i = 0; i < Vn_COUNT; i++) {
            old_size += Vs[i].FIRST.size();
        }
        for (int i = 0; i < Vn_COUNT; i++) {
            for (int j = 0; j < PRODUCTION_COUNT; j++) {
                if (P[j].Vn == Vs[i].V) {
                    k = 0;
                    ctnu = true;
                    while (ctnu && k < P[j].n) {
                        s = &(Vs[Vs_idx_of(P[j].V_list[k])].FIRST);
                        set_add_except(&Vs[i].FIRST, s, T_EPS);
                        if (!is_in_set(s, T_EPS)) ctnu = false;
                        k += 1;
                    }
                    if (ctnu) Vs[i].FIRST.insert(T_EPS);
                }
            }
            new_size += Vs[i].FIRST.size();
        }
    } while (old_size != new_size);
}
//构造符号串X1X2..Xn的FIRST集,符号串在*Xs中按顺序放置,结果放置到*first中
void make_list_FIRST(const std::vector<int> *Xs, std::set<int> *first)
{
    bool ctnu;
    unsigned int k = 0;
    ctnu = true;
    first->clear();
    while (ctnu && k < Xs->size()) {
        set_add_except(first, &Vs[Vs_idx_of(Xs->at(k))].FIRST, T_EPS);
        if (!is_in_set(&Vs[Vs_idx_of(Xs->at(k))].FIRST, T_EPS)) ctnu = false;
        k++;
    }
    if (ctnu) first->insert(T_EPS);
}
//构造Vs[]中所有元素的FOLLOW集
void make_FOLLOW()
{
    std::vector<int> Xs;
    std::set<int> first;
    int old_size, new_size;

    Vs[0].FOLLOW.insert(EOI);

    do {
        old_size = new_size = 0;
        for (int i = 0; i < Vn_COUNT; i++) {
            old_size += Vs[i].FOLLOW.size();
        }
        for (int i = 0; i < PRODUCTION_COUNT; i++) {// for each production A->X1 X2...Xn
            for (int j = 0; j < P[i].n; j++) {
                if (is_Vn(P[i].V_list[j])) {//for each Xi that is a Vn
                    //make first(Xi+1 Xi+2 Xi+3...Xn)
                    Xs.clear();
                    for (int l = j + 1; l < P[i].n; l++) {
                        Xs.push_back(P[i].V_list[l]);
                    }
                    first.clear();
                    if (!Xs.empty()) {
                        make_list_FIRST(&Xs, &first);
                        set_add_except(&Vs[Vs_idx_of(P[i].V_list[j])].FOLLOW, &first, T_EPS);
                    }
                    if (is_in_set(&first, T_EPS) || Xs.empty()) {//if t_eps in first(Xi+1Xi+2...Xi+n)
                        //add follow(A) to follow(Xi)
                        set_add(&Vs[Vs_idx_of(P[i].V_list[j])].FOLLOW, &Vs[Vs_idx_of(P[i].Vn)].FOLLOW);
                    }
                }
            }
        }
        for (int i = 0; i < Vn_COUNT; i++) {
            new_size += Vs[i].FOLLOW.size();
        }
    } while (old_size != new_size);
}
//构造分析表
void make_parsing_table()
{
    for (int i = 0; i < M_ROW_CNT; i++) {
        M_rows[i].Vns = &Vs[i];
    }
    std::vector<int> Xs;
    std::set<int>    first;
    for (int i = 0; i < M_ROW_CNT; i++) {
        for (int j = 0; j < M_COL_CNT; j++) {
            for (int k = 0; k < PRODUCTION_COUNT; k++) {
                if (P[k].Vn == M_rows[i].Vns->V) {
                    Xs.clear();
                    for (int l = 0; l < P[k].n; l++) {
                        Xs.push_back(P[k].V_list[l]);
                    }
                    make_list_FIRST(&Xs, &first);
                    //--debug start
                    for (unsigned int m = 0; m < Xs.size(); m++) {
                        printf("%d ", Xs.at(m));
                    }
                    printf(": ");
                    for (std::set<int>::iterator it = first.begin(); it != first.end(); it++) {
                        printf("%d,", *it);
                    }
                    printf("  , a = %d\n", M_cols[j]);
                    //--debug end
                    if (is_in_set(&first, M_cols[j])) {
                        M_rows[i].p[j] = &P[k];
                        break;
                    } else if (is_in_set(&first, T_EPS) && is_in_set(&M_rows[i].Vns->FOLLOW, M_cols[j])) {
                        M_rows[i].p[j] = &P[k];
                        break;
                    } else {
                        M_rows[i].p[j] = NULL;
                        continue;
                    }
                }
            }
        }
    }

}
//查找分析表M[Xm, ai]
const struct P_t* lookup_parsing_table(int Xm, int ai)
{
    for (int i = 0; i < M_ROW_CNT; i++) {
        if (M_rows[i].Vns->V == Xm) {
            for (int j = 0; j < M_COL_CNT; j++) {
                if (M_cols[j] == ai) {
                    return M_rows[i].p[j];
                }
            }
        }
    }
    return NULL;
}

void report_error()
{
    fprintf(stderr, "error!\n");
}

bool LL1_parse(FILE *ifp)
{
    int cur_cls;
    char cur_val[32];
    while (!feof(ifp)) {
        fscanf(ifp, "%d, %s", &cur_cls, cur_val);
        sym_queue.push(cls_to_vt(cur_cls, cur_val));
    }

    int Xm, ai;
    const struct P_t *p;
    parsing_stack.push(EOI);
    parsing_stack.push(N_E);
    while (true) {
        Xm = parsing_stack.top();
        ai = sym_queue.front();
        if (is_Vn(Xm)) {
            if ((p = lookup_parsing_table(Xm, ai)) != NULL) {
                parsing_stack.pop();
                for (int i = p->n - 1; i >= 0; i--) {
                    if (p->V_list[i] != T_EPS) parsing_stack.push(p->V_list[i]);
                }
            } else {
                report_error();
                return false;
            }
        } else if (Xm != EOI) {
            if (Xm == ai && ai != EOI) {
                parsing_stack.pop();
                sym_queue.pop();
            } else {
                report_error();
                return false;
            }
        } else {
            if (Xm == ai && ai == EOI) {
                return true;
            } else {
                report_error();
                return false;
            }
        }
    }
}

int main()
{
#define TEST_CNT 2
    FILE *ifps[TEST_CNT];
    FILE *ofps[TEST_CNT];
    char in[128], out[128];

    for (int i = 0; i < TEST_CNT; i++) {
        sprintf(in, "/tmp/byyl/test3_%d.in", i + 1);
        ifps[i] = fopen(in, "r");
        check_fopen(ifps[i], in);
    }
    for (int i = 0; i < TEST_CNT; i++) {
        sprintf(out, "/tmp/byyl/test3_%d.out", i + 1);
        ofps[i] = fopen(out, "w");
        check_fopen(ofps[i], out);
    }

    int i;
    for (i = 0; i < Vn_COUNT; i++) {
        Vs[i].V = Vn_table[i];
    }
    for (i = 0; i < Vt_COUNT; i++) {
        Vs[Vn_COUNT + i].V = Vt_table[i];
    }
    make_FIRST();
    for (i = 0; i < V_COUNT; i++) {
        printf("%d\t", Vs[i].V);
        for (std::set<int>::iterator it = Vs[i].FIRST.begin(); it != Vs[i].FIRST.end(); it++) {
            printf("%d ", *it);
        }
        putchar('\n');
    }
    printf("\n\n");
    make_FOLLOW();
    for (i = 0; i < V_COUNT; i++) {
        printf("%d\t", Vs[i].V);
        for (std::set<int>::iterator it = Vs[i].FOLLOW.begin(); it != Vs[i].FOLLOW.end(); it++) {
            printf("%d ", *it);
        }
        putchar('\n');
    }
    printf("\n\n");
    make_parsing_table();
    for (i = 0; i < Vn_COUNT; i++) {
        printf("%d\t", M_rows[i].Vns->V);
        for (int j = 0; j < M_COL_CNT; j++) {
            printf("%s,", M_rows[i].p[j] == NULL ? "ERROR" : M_rows[i].p[j]->p_str);
        }
        putchar('\n');
    }

    for (i = 0; i < TEST_CNT; i++) {
        if (LL1_parse(ifps[i])) {
            fprintf(ofps[i], "true");
        } else {
            fprintf(ofps[i], "false");
        }
        fclose(ifps[i]);
        fclose(ofps[i]);
    }
    return 0;
}
