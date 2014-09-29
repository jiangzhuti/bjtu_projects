#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "../clsdef.h"

/* 当前单词 */
char TOKEN[64];

/* 忽略大小写的strcmp */
/* 修改自glibc */
static int my_strcasecmp(const char *s1, const char *s2)
{
    const unsigned char *p1 = (const unsigned char *) s1;
    const unsigned char *p2 = (const unsigned char *) s2;
    int result;
    if (p1 == p2)
      return 0;
    while ((result = tolower(*p1) - tolower(*p2++)) == 0)
      if (*p1++ == '\0')
        break;
    return result;
}

static void check_fopen(FILE *fp, const char *name)
{
    if (fp == NULL) {
        fprintf(stderr, "cannot open file: %s\n", name);
        exit(EXIT_FAILURE);
    }
}

/* 报告错误 */
void report_error()
{
    fprintf(stderr, "an error occured!\n");
}

int lookup()
{
    for (unsigned int i = 0; i < RESERVED_COUNT; i++) {
        if (my_strcasecmp(reserved_table[i].name, TOKEN) == 0)
            return reserved_table[i].class_code;
    }
    return 0;
}

void out(int clscode, const char *s, FILE *ofp)
{
    fprintf(ofp, "%d, %s\n", clscode, s);
}

void scanner(FILE *ifp, FILE *ofp)
{
    char ch;
    int i, cls;
    ch = fgetc(ifp);
    while (!feof(ifp)) { //skip space
        if (isspace(ch))
            ch = fgetc(ifp);
        else
            break;
    }
    if (feof(ifp)) return;
    if (isalpha(ch)) { //ID OR RESERVED
        TOKEN[0] = ch;
        ch = fgetc(ifp);
        i = 1;
        while (isalnum(ch)) {
            TOKEN[i++] = ch;
            ch = fgetc(ifp);
        }
        TOKEN[i] = '\0';
        fseek(ifp, -1, 1);
        cls = lookup();
        if (cls == 0) out(ID, TOKEN, ofp);
        else out(cls, "", ofp);
    } else if (isdigit(ch)) { //UNSIGNED INT
        TOKEN[0] = ch;
        ch = fgetc(ifp);
        i = 1;
        while (isdigit(ch)) {
            TOKEN[i++] = ch;
            ch = fgetc(ifp);
        }
        TOKEN[i] = '\0';
        fseek(ifp, -1, 1);
        out(UINT, TOKEN, ofp);
    } else {
        switch (ch) {
        case '<':
            ch = fgetc(ifp);
            if (ch == '=') out(DSPL, "<=", ofp);
            else if (ch == '>') out(DSPL, "<>", ofp);
            else {
                fseek(ifp, -1, 1);
                out(LT, "", ofp);
            }
            break;
        case '>':
            ch = fgetc(ifp);
            if (ch == '=') out(DSPL, ">=", ofp);
            else {
                fseek(ifp, -1, 1);
                out(GT, "", ofp);
            }
            break;
        case '=':
            out(EQ, "", ofp);
            break;
        case ':':
            ch = fgetc(ifp);
            if (ch == '=') out(DSPL, ":=", ofp);
            else {
                fseek(ifp, -1, 1);
                out(MH, "", ofp);
            }
            break;
        case '/':
            ch = fgetc(ifp);
            if (ch == '*') {//comment begin
                ch = fgetc(ifp);
                while (!feof(ifp)) {
                    if (ch == '*') {
                        if (fgetc(ifp) == '/') break;//comment end
                        else {
                            fseek(ifp, -1, SEEK_CUR);
                            ch = fgetc(ifp);
                        }
                    }
                    ch = fgetc(ifp);
                }
            } else {
                fseek(ifp, -1, 1);
                out(XS, "", ofp);
            }
            break;
        case '+':
            out(SSPL, "+", ofp);
            break;
        case '-':
            out(SSPL, "-", ofp);
            break;
        case '*':
            out(SSPL, "*", ofp);
            break;
        case ';':
            out(SSPL, ";", ofp);
            break;
        case '(':
            out(SSPL, "(", ofp);
            break;
        case ')':
            out(SSPL, ")", ofp);
            break;
        default:
            report_error();
            break;
        }
    }
}

int main(void)
{
#define TEST_CNT 7//测试样本数量
    FILE *ifps[TEST_CNT];//input files
    FILE *ofps[TEST_CNT];//output files
    char in[128], out[128];

    //打开测试文件
    for (int i = 0; i < TEST_CNT; i++) {
        sprintf(in, "test1_%d.in", i + 1);
        ifps[i] = fopen(in, "r");
        check_fopen(ifps[i], in);
    };
    for (int i = 0; i < TEST_CNT; i++) {
        sprintf(out, "test1_%d.out", i + 1);
        ofps[i] = fopen(out, "w");
        check_fopen(ofps[i], out);
    }

    //测试
    for (int i = 0; i < TEST_CNT; i++) {
        while (!feof(ifps[i]))
            scanner(ifps[i], ofps[i]);
        fprintf(ofps[i], "%d, %c\n", EOI, '#');
        fclose(ifps[i]);
        fclose(ofps[i]);
    }
    return 0;
}
