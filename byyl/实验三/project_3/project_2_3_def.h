#ifndef PROJECT_2_3_DEFS_H
#define PROJECT_2_3_DEFS_H

/* symbols(terminal or nonterminal) defines */
#define Vn_COUNT   7
#define Vt_COUNT   8
#define V_COUNT    (Vn_COUNT + Vt_COUNT)
//nonterminals
#define N_E    0x101
#define N_EP   0x102
#define N_T    0x103
#define N_TP   0x104
#define N_F    0x105
#define N_A    0x106
#define N_M    0x107
//terminals
#define T_ID   0x108 //same as ID
#define T_LP   0x109
#define T_RP   0x10A
#define T_PLUS 0x10B
#define T_SUB  0x10C
#define T_MUL  0x10D
#define T_DIV  0x10E //same as XS
#define T_EPS  0x10F

/* all unknown symbol/class */
#define UNKNOWN 0xFFFFF

//symbol table
const static int Vn_table[Vn_COUNT] = {N_E, N_EP, N_T, N_TP, N_F, N_A, N_M};
const static int Vt_table[Vt_COUNT] = {T_ID, T_LP, T_RP, T_PLUS, T_SUB, T_MUL, T_DIV, T_EPS};

#include <set>//for std::set
//symbol with FIRST set and FOLLOW set
struct Vset_t {
    int V;
    std::set<int> FIRST;
    std::set<int> FOLLOW;
};
//production type
struct P_t {
    int Vn;
    int V_list[32];
    int n;//number of symbols in V_list
    const char *p_str;
};
#define PRODUCTION_COUNT 12

//productions
const static struct P_t P[PRODUCTION_COUNT] =
{
    {N_E,  {N_T, N_EP},       2, "E->TE\'"},
    {N_EP, {N_A, N_T, N_EP},  3, "E\'->ATE'"},
    {N_EP, {T_EPS},           1, "E\'->eps"},
    {N_T,  {N_F, N_TP},       2, "T->FT\'"},
    {N_TP, {N_M, N_F, N_TP},  3, "T\'->MFT\'"},
    {N_TP, {T_EPS},           1, "T\'->eps"},
    {N_F,  {T_LP, N_E, T_RP}, 3, "F->(E)"},
    {N_F,  {T_ID},            1, "F->i"},
    {N_A,  {T_PLUS},          1, "A->+"},
    {N_A,  {T_SUB},           1, "A->-"},
    {N_M,  {T_MUL},           1, "M->*"},
    {N_M,  {T_DIV},           1, "M->/"}
};

/* functions */

static inline bool is_Vn(int v)
{
    return (v >= N_E && v <= N_M);
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

#endif // PROJECT_2_3_DEFS_H
