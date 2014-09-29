#include <stdio.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 栈操作返回值的定义
 */

#define STATUS                     long

#define SUCCESS                       1
#define OVERFLOW                     -2
#define ERROR                         0

/*
 * 栈结构的实现
 */

#define STACK_INIT_SIZE             100
#define STACK_INCREMENT              10

typedef char stack_elem_type;
typedef struct {
    stack_elem_type  *base;
    stack_elem_type   *top;
    int               size;
} sq_stack;

STATUS init_stack(sq_stack *s)
{
    s->base = malloc(STACK_INIT_SIZE * sizeof(stack_elem_type));
    if (s->base == NULL)
        exit(OVERFLOW);
    s->top = s->base;
    s->size = STACK_INIT_SIZE;
    return SUCCESS;
}

int stack_empty(sq_stack *s)
{
    return s->base == s->top;
}

STATUS push(sq_stack *s, const stack_elem_type *e)
{
    if (s->top - s->base >= s->size) {
        s->base = realloc(s->base, (s->size + STACK_INCREMENT) * sizeof(stack_elem_type));
        if (s->base == NULL) exit(OVERFLOW);
        s->top = s->base + s->size;
        s->size += STACK_INCREMENT;
    }
    *(s->top) = *e;
    s->top++;
    return SUCCESS;
}

STATUS pop(sq_stack *s, stack_elem_type *e)
{
    if (s->top == s->base) return ERROR;
    s->top--;
    *e = *(s->top);
    return SUCCESS;
}

STATUS get_top(const sq_stack *s, stack_elem_type *e)
{
    if (s->top == s->base) return ERROR;
    *e = *(s->top - 1);
    return SUCCESS;
}

STATUS destory_stack(sq_stack *s)
{
    free(s->base);
    s->base = s->top = NULL;
    s->size = -1;
    return SUCCESS;
}

/*
 * 表达式转换部分
 */

const char operator_table[6] = {'+', '-', '*', '/', '(', ')'};
const int operator_count = 6;
const char terminator = '#';

int optr_rank(char optr)
{
    switch (optr) {
    case '+':
    case '-':
        return 1;
        break;
    case '*':
    case '/':
        return 2;
        break;
    default:
        return 0;
    }
}

int is_operator(char c)
{
    int i;
    for (i = 0; i < operator_count; i++) {
        if (operator_table[i] == c)
            return 1;
    }
    return 0;
}

static char suffix[128], exp[128], lval;

void transform()
{
    sq_stack s;
    stack_elem_type tmp;
    int i = 0, j = 0;
    init_stack(&s);
    push(&s, &terminator);
    while (!stack_empty(&s)) {
        if (!is_operator(exp[i]) && exp[i] != terminator) {
            suffix[j] = exp[i];
            j++;
        } else {
            switch (exp[i]) {
            case '(':
                push(&s, &exp[i]);
                break;
            case ')':
                pop(&s, &tmp);
                while (tmp != '(') {
                    suffix[j] = tmp;
                    j++;
                    pop(&s, &tmp);
                }
                break;
            default:
                while (get_top(&s, &tmp) == SUCCESS && optr_rank(tmp) > optr_rank(exp[i])) {
                    suffix[j] = tmp;
                    j++;
                    pop(&s, &tmp);
                }
                if (exp[i] != terminator) push(&s, &exp[i]);
                break;
            }
        }
        if (exp[i] != terminator) {
            i++;
        } else {
            pop(&s, &tmp);
            suffix[j] = tmp;
            j++;
        }
    }
    suffix[j] = '\0';
    destory_stack(&s);
}

struct quad_t {
    char op;
    char a1;
    char a2;
    char rs;
};

void output_quad(FILE *fp, struct quad_t *quad)
{
    fprintf(fp, "%c, %c, %c, %c\n", quad->op, quad->a1, quad->a2, quad->rs);
}

int find_and_del_quad(struct quad_t *quad)
{
    static char T = 'A';
    int i, j, k;
    i = 2;
    while (suffix[i] != terminator) {
        if (suffix[i] == '+' || suffix[i] == '-' || suffix[i] == '*' || suffix[i] == '/') {
            for (j = i - 1; j > 0; --j) {
                if (isspace(suffix[j])) continue;
                else break;
            }
            if (!isalnum(suffix[j])) continue;
            for (k = j - 1; k >= 0; --k) {
                if (isspace(suffix[k])) continue;
                else break;
            }
            if (!isalnum(suffix[k])) continue;
            quad->op = suffix[i];
            quad->a1 = suffix[k];
            quad->a2 = suffix[j];
            quad->rs = T;
            for (; k <= i; ++k) {
                suffix[k] = ' ';
            }
            suffix[--k] = T;
            T++;
            return 0;
        }
        ++i;
    }
    return T;
}

void gen_quad(FILE *fp)
{
    struct quad_t quad;
    int T;
    while ((T = find_and_del_quad(&quad)) == 0) {
        output_quad(fp, &quad);
    }
    quad.op = '=';
    quad.a1 = T - 1;
    quad.a2 = ' ';
    quad.rs = lval;
    output_quad(fp, &quad);
}

int main()
{
    FILE *fpin  = fopen("/tmp/byyl/test_6.in", "r");
    FILE *fpout = fopen("/tmp/byyl/test_6.out", "w");
    fscanf(fpin, "%c=%s", &lval, exp);
    transform(suffix, exp);
    puts(suffix);
    gen_quad(fpout);
    fclose(fpout);
    return 0;
}
