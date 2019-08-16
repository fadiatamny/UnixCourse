#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

struct _foo_struct
{
    int some_status;
    char *some_string;
} foo_arg;

void *foo(void *arg)
{
    printf("foo. returning...\n");
    foo_arg.some_status = 0;
    foo_arg.some_string = "Hey";
    return &foo_arg;
}

void *bar(void *arg)
{
    printf("bar. exiting...\n");
    pthread_exit((void *)1);
}
int main(int argc, const char *argv[])
{
    pthread_t tid1, tid2;
    void *ret;
    struct _foo_struct *fs;
    if (pthread_create(&tid1, NULL, foo,
                       NULL))
        return 1;
    if (pthread_create(&tid2, NULL, bar,
                       NULL))
        return 1;
    if (pthread_join(tid1, &ret))
    {
        fprintf(stderr, "Could not join Foo Thread\n");
        return 1;
    }
    fs = (struct _foo_struct *)ret;
    printf("Foo thread exited with status: (% d, % s)\n", fs->some_status,
           fs->some_string);
    if (pthread_join(tid2, &ret))
    {
        fprintf(stderr, "Could not join with Bar Thread\n");
        return 1;
    }
    printf("Bar thread exited with status:%d\n", (int)ret);
    return 0;
}