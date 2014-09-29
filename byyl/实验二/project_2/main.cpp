#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "clsdef.h"
#include "project_2_3_def.h"

/* 当前符号的类别编码 */
int cur_cls;
/* 当前的符号 */
char cur_val[128];

/* 当前输入文件指针 */
FILE *ifp;

static void check_fopen(FILE *fp, const char *name)
{
    if (fp == NULL) {
        fprintf(stderr, "cannot open file: %s\n", name);
        exit(EXIT_FAILURE);
    }
}

bool match(int s)
{
    return cls_to_vt(cur_cls, cur_val) == s;
}

void advance()
{
    fscanf(ifp, "%d, %s", &cur_cls, cur_val);
    if (feof(ifp)) {
        cur_cls = EOI;
        strcpy(cur_val, "#");
    }
}

bool check_E();
bool check_T();
bool check_Ep();
bool check_A();
bool check_Tp();
bool check_M();
bool check_F();

bool check_E()
{
    if (!check_T())
        return false;
    if (!check_Ep())
        return false;
    if (!match(EOI) && !match(T_RP)) {
        check_E();
    }
    return true;
}

bool check_Ep()
{
    if (check_A()) {
        advance();
        if (!check_T())
            return false;
        if (!check_Ep())
            return false;
        return true;
    }
    return true;
}

bool check_T()
{
    if (!check_F())
        return false;
    if (!check_Tp())
        return false;
    return true;
}

bool check_Tp()
{
    if (check_M()) {
        advance();
        if (!check_F())
            return false;
        if (!check_Tp())
            return false;
        return true;
    }
    return true;
}

bool check_F()
{
    if (match(T_ID)) {
        advance();
        return true;
    }
    else if (match(T_LP)) {
        advance();
        if (!check_E())
        return false;
        if (match(T_RP)) {
            advance();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool check_A()
{
    return (match(T_PLUS) || match(T_SUB));
}

bool check_M()
{
    return (match(T_MUL) || match(T_DIV));
}

int main(void)
{
#define TEST_CNT 2
    FILE *ifps[TEST_CNT];
    FILE *ofps[TEST_CNT];

    char in[128], out[128];

    for (int i = 0; i < TEST_CNT; i++) {
        sprintf(in, "/tmp/byyl/test2_%d.in", i + 1);
        ifps[i] = fopen(in, "r");
        check_fopen(ifps[i], in);
    }
    for (int i = 0; i < TEST_CNT; i++) {
        sprintf(out, "/tmp/byyl/test2_%d.out", i + 1);
        ofps[i] = fopen(out, "w");
        check_fopen(ofps[i], out);
    }
    for (int i = 0; i < TEST_CNT; i++) {
        ifp = ifps[i];
        advance();
        if (check_E()) {
            fprintf(ofps[i], "true");
        } else {
            fprintf(ofps[i], "false");
        }
        fclose(ifps[i]);
        fclose(ofps[i]);
    }
    return 0;
}
