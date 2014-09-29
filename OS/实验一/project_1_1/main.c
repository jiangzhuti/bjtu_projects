#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

//禁止优化对变量的操作（主要是别写到寄存器里）
volatile int accnt1 = 0;
volatile int accnt2 = 0;

volatile int c1 = 0, c2 = 0, will_wait;

pthread_mutex_t mutex2;//printf不可重入，需要把printf锁住

void *run(void *arg)
{
    int count = 10000000;
    struct timeval tvstart1, tvend1, tvstart2, tvend2;

    int counter = 0;
    int tmp1, tmp2, r;
    if (*(int *)arg == 1) goto p1;//把线程分开执行
    else goto p2;

p1:  while (1) {
        gettimeofday(&tvstart1, NULL);//开始计时
        do {
            c1 = 0;
            c1 = 1;
            will_wait = 1;
            while (c2 && will_wait == 1);
            tmp1 = accnt1;
            tmp2 = accnt2;
            r = rand();
            accnt1 = tmp1 + r;
            accnt2 = tmp2 - r;
            counter++;
        } while (accnt1 + accnt2 == 0 && count--);
        c1 = 0;
        gettimeofday(&tvend1, NULL);//计时结束
        pthread_mutex_lock(&mutex2);
        printf("difftime is : %ld\n", 1000000 * (tvend1.tv_sec-tvstart1.tv_sec)+ tvend1.tv_usec-tvstart1.tv_usec);
        printf("%d\n", counter);
        pthread_mutex_unlock(&mutex2);
        return NULL;
    }
p2:  while (1) {
        gettimeofday(&tvstart2, NULL);//开始计时
        do {
            c2 = 0;
            c2 = 1;
            will_wait = 2;
            while (c1 && will_wait == 2);
            tmp1 = accnt1;
            tmp2 = accnt2;
            r = rand();
            accnt1 = tmp1 + r;
            accnt2 = tmp2 - r;
            counter++;
        } while (accnt1 + accnt2 == 0 && count--);
        c2 = 0;
        gettimeofday(&tvend2, NULL);//计时结束
        pthread_mutex_lock(&mutex2);
        printf("difftime is : %ld\n", 1000000 * (tvend2.tv_sec-tvstart2.tv_sec)+ tvend2.tv_usec-tvstart2.tv_usec);
        printf("%d\n", counter);
        pthread_mutex_unlock(&mutex2);
        return NULL;
    }
    return NULL;
}

int main(void)
{
    int t1 = 1, t2 = 2;
    pthread_t thread1, thread2;
    pthread_mutex_init(&mutex2, NULL);
    pthread_create(&thread1, NULL, run, &t1);
    pthread_create(&thread2, NULL, run, &t2);
    //等待线程结束
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}

