#include <stdio.h>
#include <pthread.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *inc_count_thread(void *f)
{
    int i, *count;
    count = (int *)f;
    for (i = 0; i < 1000000; ++i)
    {
        pthread_mutex_lock(&mutex);
        ++(*count);
        pthread_mutex_unlock(&mutex);
    }
        
    return NULL;
}

int main(int argc, const char *argv[])
{
    int count = 0;
    pthread_t tid1, tid2;
    if (pthread_create(&tid1, NULL,
                       inc_count_thread, &count))
        return 1;
    if (pthread_create(&tid2, NULL,
                       inc_count_thread, &count))
        return 1;
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("Count=%d\n", count);
    return 0;
}