#ifndef PROJECT_4_DEF_H
#define PROJECT_4_DEF_H

/* symbols(terminal or nonterminal) defines */
#define Vn_COUNT   3
#define Vt_COUNT   7
#define V_COUNT    (Vn_COUNT + Vt_COUNT)
//nonterminals
#define N_E    0x201
#define N_T    0x202
#define N_F    0x203
//terminals
#define T_ID   0x204 //same as ID
#define T_LP   0x205
#define T_RP   0x206
#define T_PLUS 0x207
#define T_SUB  0x208
#define T_MUL  0x209
#define T_DIV  0x20A //same as XS

/* all unknown symbol/class */
#define UNKNOWN 0xFFFFF

/* 算符优先关系 */
#define DAYU   0x211
#define XIAOYU 0x212
#define DENGYU 0x213

//symbol table
const static int Vn_table[Vn_COUNT] = {N_E, N_T, N_F};
const static int Vt_table[Vt_COUNT] = {T_ID, T_LP, T_RP, T_PLUS, T_SUB, T_MUL, T_DIV};

#include <set>//for std::set
//symbol with FIRSTVT set and LASTVT set
struct Vset_t {
    int V;
    std::set<int> FIRSTVT;
    std::set<int> LASTVT;
};
//production type
struct P_t {
    int Vn;
    int V_list[32];
    int n;//number of symbols in V_list
    const char *p_str;
};
#define PRODUCTION_COUNT 8

//productions
const static struct P_t P[PRODUCTION_COUNT] =
{
    {N_E, {N_E, T_PLUS, N_T}, 3, "E->E+T"},
    {N_E, {N_E, T_SUB, N_T},  3, "E->E-T"},
    {N_E, {N_T},              1, "E->T"  },
    {N_T, {N_T, T_MUL, N_F},  3, "T->T*F"},
    {N_T, {N_T, T_DIV, N_F},  3, "T->T/F"},
    {N_T, {N_F},              1, "T->F"  },
    {N_F, {T_LP, N_E, T_RP},  3, "F->(E)"},
    {N_F, {T_ID},             1, "F->i"  },
};

/* functions */

static inline bool is_Vn(int v)
{
    return (v >= N_E && v <= N_F);
}

static inline bool is_Vt(int v)
{
    return (v >= T_ID && v <= T_DIV);
}

#include <cstring>//for strcmp
/* 根据类别码和值确定非终结符(和EOI) */
static inline int cls_to_vt(int cls, const char *val)
{
    switch (cls) {
    case ID://user defined id
        return T_ID;
    case SSPL://单字符分隔符
        if (!strcmp(val, "(")) return T_LP;
        else if (!strcmp(val, ")")) return T_RP;
        else if (!strcmp(val, "+")) return T_PLUS;
        else if (!strcmp(val, "-")) return T_SUB;
        else if (!strcmp(val, "*")) return T_MUL;
        else if (!strcmp(val, "/")) return T_DIV;
        else return UNKNOWN;
    case XS://斜竖
        return T_DIV;
    case EOI:
        return EOI;
    default:
        return UNKNOWN;
    }
}

#endif // PROJECT_4_DEF_H
