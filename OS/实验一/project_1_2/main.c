#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int accnt1 = 0;
int accnt2 = 0;

void *run(void *arg)
{
    int counter = 0;
    int tmp1, tmp2, r;
    (void)(arg == arg);//防止出现无用参数的warning-_-!!

    do {
        tmp1 = accnt1;
        tmp2 = accnt2;
        r = rand();
        accnt1 = tmp1 + r;
        accnt2 = tmp2 - r;
        counter++;
    } while (accnt1 + accnt2 == 0);

    printf("%d\n", counter);
    return NULL;
}

int main(void)
{
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, run, "thread1");
    pthread_create(&thread2, NULL, run, "thread2");
    //等待线程结束
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}

