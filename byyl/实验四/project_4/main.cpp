#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <set>
#include <stack>
#include <vector>
#include <queue>
#include "clsdef.h"
#include "project_4_def.h"

static Vset_t Vs[Vn_COUNT];
static int OPG_matrix[Vt_COUNT + 1][Vt_COUNT + 1];
static int OPG_matrix_head[Vt_COUNT + 1];
static int f_values[Vt_COUNT + 1], g_values[Vt_COUNT + 1];
int parse_stack[128];
static std::queue<int> sym_queue;

static void check_fopen(FILE *fp, const char *name)
{
    if (fp == NULL) {
        fprintf(stderr, "cannot open file: %s\n", name);
        exit(EXIT_FAILURE);
    }
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
void single_firstVT(int Vs_idx1, int Vs_idx2)
{
    int i, j;
    for (i = 0; i < PRODUCTION_COUNT; i++) {
        if (P[i].Vn == Vs[Vs_idx2].V) {
            for (j = 0; j < P[i].n; j++) {
                if (is_Vt(P[i].V_list[j])) {
                    Vs[Vs_idx1].FIRSTVT.insert(P[i].V_list[j]);
                    break;
                }
            }
            if (j == P[i].n) {
                single_firstVT(Vs_idx1, Vs_idx_of(P[i].V_list[0]));
            }
        }
    }
}

void firstVT()
{
    for (int i = 0; i < Vn_COUNT; i++) {
        single_firstVT(i, i);
    }
}

void single_lastVT(int Vs_idx1, int Vs_idx2)
{
    int i, j;
    for (i = 0; i < PRODUCTION_COUNT; i++) {
        if (P[i].Vn == Vs[Vs_idx2].V) {
            for (j = P[i].n - 1; j > -1; j--) {
                if (is_Vt(P[i].V_list[j])) {
                    Vs[Vs_idx1].LASTVT.insert(P[i].V_list[j]);
                    break;
                }
            }
            if (j == -1) {
                single_lastVT(Vs_idx1, Vs_idx_of(P[i].V_list[P[i].n - 1]));
            }
        }
    }
}

void lastVT()
{
    for (int i = 0; i < Vn_COUNT; i++) {
        single_lastVT(i, i);
    }
}

void print_firstVT()
{
    for (int i = 0; i < Vn_COUNT; i++) {
        printf("%x: ", Vs[i].V);
        for (std::set<int>::iterator it = Vs[i].FIRSTVT.begin(); it != Vs[i].FIRSTVT.end(); it++) {
            printf("%x,", *it);
        }
        printf("\n");
    }
}

void print_lastVT()
{
    for (int i = 0; i < Vn_COUNT; i++) {
        printf("%x: ", Vs[i].V);
        for (std::set<int>::iterator it = Vs[i].LASTVT.begin(); it != Vs[i].LASTVT.end(); it++) {
            printf("%x,", *it);
        }
        printf("\n");
    }
}

int next_Vt(int idx, const struct P_t *p)
{
    for (int i = idx + 1; i < p->n; i++) {
        if (is_Vt(p->V_list[i])) return p->V_list[i];
    }
    return -1;
}

void set_prec(int Vt1, int Vt2, int prec)
{
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        if (OPG_matrix_head[i] == Vt1) {
            for (int j = 0; j < Vt_COUNT + 1; j++) {
                if (OPG_matrix_head[j] == Vt2) {
                    OPG_matrix[i][j] = prec;
                }
            }
        }
    }
}

int get_prec(int Vt1, int Vt2)
{
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        if (OPG_matrix_head[i] == Vt1) {
            for (int j = 0; j < Vt_COUNT + 1; j++) {
                if (OPG_matrix_head[j] == Vt2) {
                    return OPG_matrix[i][j];
                }
            }
        }
    }
    return -1;
}

void make_OPG_matrix()
{
    int t;
    for (int i = 0; i < PRODUCTION_COUNT; i++) {
        for (int j = 0; j < P[i].n; j++) {
            if (is_Vt(P[i].V_list[j])) {
                if ((t = next_Vt(j, &P[i])) != -1) {
                    set_prec(P[i].V_list[j], t, DENGYU);
                }
                if (j < P[i].n - 1 && is_Vn(P[i].V_list[j + 1])) {
                    for (int k = 0; k < Vn_COUNT; k++) {
                        if (Vs[k].V == P[i].V_list[j + 1]) {
                            for (std::set<int>::iterator it = Vs[k].FIRSTVT.begin(); it != Vs[k].FIRSTVT.end(); it++) {
                                set_prec(P[i].V_list[j], *it, XIAOYU);
                            }
                        }
                    }
                }
                if (j != 0 && is_Vn(P[i].V_list[j - 1])) {
                    for (int k = 0; k < Vn_COUNT; k++) {
                        if (Vs[k].V == P[i].V_list[j - 1]) {
                            for (std::set<int>::iterator it = Vs[k].LASTVT.begin(); it != Vs[k].LASTVT.end(); it++) {
                                set_prec(*it, P[i].V_list[j], DAYU);
                            }
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        if (OPG_matrix_head[i] != T_RP && OPG_matrix_head[i] != EOI) {
            OPG_matrix[Vt_COUNT][i] = XIAOYU;
        }
        if (OPG_matrix_head[i] != T_LP && OPG_matrix_head[i] != EOI) {
            OPG_matrix[i][Vt_COUNT] = DAYU;
        }
    }
}

void print_OPM()
{
    putchar('\t');
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        printf("%x\t", OPG_matrix_head[i]);
    }
    putchar('\n');
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        printf("%x\t", OPG_matrix_head[i]);
        for (int j = 0; j < Vt_COUNT + 1; j++) {
            printf("%x\t", OPG_matrix[i][j]);
        }
        putchar('\n');
    }
}

void make_prec_func()
{
    bool f;
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        f_values[i] = g_values[i] = 1;
    }
     do {
        f = false;
        for (int i = 0; i < Vt_COUNT + 1; i++) {
            for (int j = 0; j < Vt_COUNT + 1; j++) {
                printf("%x,%x\n", OPG_matrix_head[i], OPG_matrix_head[j]);
                if (get_prec(OPG_matrix_head[i], OPG_matrix_head[j]) == DAYU && f_values[i] <= g_values[j]) {
                    f_values[i] = g_values[j] + 1;
                    f = true;
                } else if (get_prec(OPG_matrix_head[i], OPG_matrix_head[j]) == XIAOYU && f_values[i] >= g_values[j]) {
                    g_values[j] = f_values[i] + 1;
                    f = true;
                } else if (get_prec(OPG_matrix_head[i], OPG_matrix_head[j]) == DENGYU && f_values[i] != g_values[j]) {
                    f_values[i] > g_values[j] ? g_values[j] = f_values[i] : f_values[i] = g_values[j];
                    f = true;
                }
            }
        }
    } while (f);
}

void print_pf()
{
    printf("\n\n");
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        printf("%x\t", OPG_matrix_head[i]);
    }
    putchar('\n');
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        printf("%x\t", f_values[i]);
    }
    putchar('\n');
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        printf("%x\t", g_values[i]);
    }
    putchar('\n');
}

int f(int Vt)
{
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        if (OPG_matrix_head[i] == Vt) {
            return f_values[i];
        }
    }
    return -1;
}
int g(int Vt)
{
    for (int i = 0; i < Vt_COUNT + 1; i++) {
        if (OPG_matrix_head[i] == Vt) {
            return g_values[i];
        }
    }
    return -1;
}

bool OPG_parser(FILE *ifp)
{
    int cur_cls;
    char cur_val[32];
    while (!feof(ifp)) {
        fscanf(ifp, "%d, %s", &cur_cls, cur_val);
        sym_queue.push(cls_to_vt(cur_cls, cur_val));
    }
    int i = 0, begin, r;
    parse_stack[0] = EOI;
    do {
        r = sym_queue.front();
        sym_queue.pop();
        if (is_Vt(parse_stack[i]) || parse_stack[i] == EOI) begin = i;
        else begin = i - 1;
        while (f(parse_stack[begin]) > g(r)) {
            int tmp;
            do {
                tmp = parse_stack[begin];
                (is_Vt(parse_stack[begin - 1]) || parse_stack[begin - 1] == EOI)? begin-- : begin -=2;
            } while (!(f(parse_stack[begin]) < g(tmp)));
            i = begin + 1;
            parse_stack[i] = N_E;
        }
        if (f(parse_stack[begin]) <= g(r)) {
            parse_stack[++i] = r;
        } else {
            return false;
        }
    } while(r != EOI);
    if (parse_stack[0] == EOI && parse_stack[1] == N_E && parse_stack[2] == EOI) {
        return true;
    } else {
        return false;
    }
}

int main()
{
#define TEST_CNT 2
    FILE *ifps[TEST_CNT];
    FILE *ofps[TEST_CNT];

    char in[128], out[128];

    for (int i = 0; i < TEST_CNT; i++) {
        sprintf(in, "/tmp/byyl/test4_%d.in", i + 1);
        ifps[i] = fopen(in, "r");
        check_fopen(ifps[i], in);
    }
    for (int i = 0; i < TEST_CNT; i++) {
        sprintf(out, "/tmp/byyl/test4_%d.out", i + 1);
        ofps[i] = fopen(out, "w");
        check_fopen(ofps[i], out);
    }
    for (int i = 0; i < Vn_COUNT; i++) {
        Vs[i].V = Vn_table[i];
    }
    for (int i = 0; i < Vt_COUNT; i++) {
        OPG_matrix_head[i] = Vt_table[i];
    }
    OPG_matrix_head[Vt_COUNT] = EOI;
    firstVT();
    print_firstVT();
    lastVT();
    print_lastVT();
    make_OPG_matrix();
    print_OPM();
    make_prec_func();
    print_pf();
    for (int i = 0; i < TEST_CNT; i++) {
        if (OPG_parser(ifps[i])) {
            fprintf(ofps[i], "true");
        } else {
            fprintf(ofps[i], "false");
        }
        fclose(ifps[i]);
        fclose(ofps[i]);
    }
    return 0;
}

