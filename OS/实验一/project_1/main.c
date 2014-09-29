#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

//禁止优化对变量的操作（主要是别写到寄存器里）
volatile int accnt1 = 0;
volatile int accnt2 = 0;

pthread_mutex_t mutex;
pthread_mutex_t mutex2;//printf不可重入，需要把printf锁住

void *run(void *arg)
{
    int counter = 0;
    int tmp1, tmp2, r;
    int count = 10000000;
    struct timeval tvstart, tvend;
    (void)(arg == arg);//防止出现无用参数的warning-_-!!

    /* 逻辑上讲最小的粒度就是循环执行一次了吧，只能这样加锁 */
    pthread_mutex_lock(&mutex);
    gettimeofday(&tvstart, NULL);//开始计时
    do {
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        tmp1 = accnt1;
        tmp2 = accnt2;
        r = rand();
        accnt1 = tmp1 + r;
        accnt2 = tmp2 - r;
        counter++;
    } while (accnt1 + accnt2 == 0 && count--);
    pthread_mutex_unlock(&mutex);
    gettimeofday(&tvend, NULL);//停止计时
    pthread_mutex_lock(&mutex2);
    printf("difftime is : %ld\n", 1000000 * (tvend.tv_sec-tvstart.tv_sec)+ tvend.tv_usec-tvstart.tv_usec);
    printf("%d\n", counter);
    pthread_mutex_unlock(&mutex2);
    return NULL;
}

int main(void)
{
    pthread_t thread1, thread2;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_create(&thread1, NULL, run, "thread1");
    pthread_create(&thread2, NULL, run, "thread2");
    //等待线程结束
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}

