#include <stdio.h>
/**
 * 两个读线程读取数据，一个写线程更新数据
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define READ_THREAD  0
#define WRITE_THREAD 1

int g_data = 0;
pthread_rwlock_t g_rwlock;

void *func(void *pdata)
{
    int data = (int)pdata;

    while (1) {
        if (READ_THREAD == data) {
            pthread_rwlock_rdlock(&g_rwlock);
            printf("-----%d------ %d\n", pthread_self(), g_data);
            sleep(1);
            pthread_rwlock_unlock(&g_rwlock);
            sleep(1);
        }
        else {
            pthread_rwlock_wrlock(&g_rwlock);
            g_data++;
            printf("add the g_data\n");
            pthread_rwlock_unlock(&g_rwlock);
            sleep(1);
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t t1, t2, t3;

    pthread_rwlock_init(&g_rwlock, NULL);

    pthread_create(&t1, NULL, func, (void *)READ_THREAD);
    pthread_create(&t2, NULL, func, (void *)READ_THREAD);
    pthread_create(&t3, NULL, func, (void *)WRITE_THREAD);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    pthread_rwlock_destroy(&g_rwlock);

    return 0;
}
