#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "../libsimplelog/libsimplelog.h"

#define IN
#define OUT

#define LOG_FILE "log.log"

void get_rand_int(OUT int *val)
{
    *val = rand();
}

void get_rand_long(OUT long *val)
{
    *val = (long)rand();
}

void get_rand_float(OUT float *val)
{
    *val = (float)rand() / (float)rand();
}

void get_rand_char(OUT char *val)
{
    *val = (char)((rand() % 26) + 'a');//限制范围小写
}

void get_rand_string(OUT char val[], OUT int *len)
{
    static int i, j;
    i = (rand() % 64);//限制长度
    *len = i + 1;
    j = 0;
    while (i--) {
        val[j] = (char)((rand() % 26) + 'A');//限制范围大写
        j++;
    }
    val[*len - 1] = '\0';
}

void test_log(int seed)
{
    int intu = 3;
    long longu = 123l;
    float floatu = 4.1;
    char charu = 'a';
    char stringu[64];
    int unit_count, record_count, stru_len;
    int i, j;
    unsigned char utype[] = {INT_TYPE, LONG_TYPE, CHAR_TYPE, FLOAT_TYPE, STRING_TYPE};

    srand(seed);
    record_count = (rand() % 10) + 1;//随机写的记录数
    for (i = 0; i < record_count; i++) {
        unit_count = (rand() % 5) + 1;//随机写的每条记录的项数
        for (j = 0; j < unit_count; j++) {
            switch (utype[rand() % 5]) {
            case INT_TYPE:
                get_rand_int(&intu);
                log_add_unit(INT_TYPE, sizeof(int), &intu);
                break;
            case LONG_TYPE:
                get_rand_long(&longu);
                log_add_unit(LONG_TYPE, sizeof(long), &longu);
                break;
            case CHAR_TYPE:
                get_rand_char(&charu);
                log_add_unit(CHAR_TYPE, sizeof(char), &charu);
                break;
            case FLOAT_TYPE:
                get_rand_float(&floatu);
                log_add_unit(FLOAT_TYPE, sizeof(float), &floatu);
                break;
            case STRING_TYPE:
                get_rand_string(stringu, &stru_len);
                log_add_unit(STRING_TYPE, stru_len, stringu);
                break;
            default:
                exit(EXIT_FAILURE);
                break;
            }
        }
        log_write(LOG_FILE);
    }
}

void test(int chcount)
{
    pid_t pid;
    int is_parent = 1;
    unsigned int seed = time(NULL);
    int status;
    while (chcount--) {
        seed++;
        if (!is_parent) break;
        if ((pid = fork()) < 0) {
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            //printf("%d\n", getpid());
            is_parent = 0;
            test_log(seed);
        } else {
            //wait(&status);
        }
    }
    while(wait(&status) != -1);//等待所有进程
}

int main(int argc, char **argv)
{
    int arg;
    int chcount;
    char from[64], to[64], tmp[128];
    while ((arg = getopt(argc, argv, "c:v:")) != -1) {
        switch (arg) {
        case 'c'://子进程个数
            chcount = atoi(optarg);
            test(chcount);
            return 0;
            break;
        case 'v'://查看日志的时间段
            strcpy(tmp, optarg);
            strcpy(from, strtok(tmp, ","));
            strcpy(to, strtok(NULL, ","));
            //printf("%s %s\n", from, to);
            log_view(from, to, LOG_FILE);
            break;
        default:
            printf("%c\n", arg);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
