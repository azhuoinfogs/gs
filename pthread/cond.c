#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define err_sys(msg) \
    do { perror(msg); exit(-1); } while(0)
#define err_exit(msg) \
    do { fprintf(stderr, msg); exit(-1); } while(0)

pthread_cond_t cond;

void *r1(void *arg)
{
    pthread_mutex_t* mutex = (pthread_mutex_t *)arg;
    static int cnt = 10;

    while(cnt--)
    {
        printf("r1: I am wait.\n");
        pthread_mutex_lock(mutex);
        pthread_cond_wait(&cond, mutex); /* mutex参数用来保护条件变量的互斥锁，调用pthread_cond_wait前mutex必须加锁 */
        pthread_mutex_unlock(mutex);
    }
    return "r1 over";
}

void *r2(void *arg)
{
    pthread_mutex_t* mutex = (pthread_mutex_t *)arg;
    static int cnt = 10;

    while(cnt--)
    {
        //pthread_mutex_lock(mutex); //这个地方不用加锁操作就行
        printf("r2: I am send the cond signal.\n");
        pthread_cond_signal(&cond);
        //pthread_mutex_unlock(mutex);
        sleep(1);
    }
    return "r2 over";
}

int main(void)
{
    pthread_mutex_t mutex;
    pthread_t t1, t2;
    char* p1 = NULL;
    char* p2 = NULL;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&t1, NULL, r1, &mutex);
    pthread_create(&t2, NULL, r2, &mutex);

    pthread_join(t1, (void **)&p1);
    pthread_join(t2, (void **)&p2);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    printf("s1: %s\n", p1);
    printf("s2: %s\n", p2);

    return 0;
}
