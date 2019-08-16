#include <pthread.h>
#include <stdio.h>
struct _foo_struct
{
    int some_status;
    char *some_string;
} foo_arg;
void clean_foo(void *arg)
{
    printf("%s\n", (char *)arg);
    foo_arg.some_status = 0;
    foo_arg.some_string = NULL;
}
void clean_nothing(void *arg)
{
    printf("%s\n", (char *)arg);
}
void *foo(void *arg)
{
    // pthread_cleanup_push(clean_foo, "Cleaning Foo.");
    // pthread_cleanup_push(clean_nothing, "Cleaning Nothing.");
    printf("foo. returning...\n");
    foo_arg.some_status = 10;
    foo_arg.some_string = "Hey";
    pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
    pthread_t tid1, tid2;
    void *ret;
    struct _foo_struct *fs;
    if (pthread_create(&tid1, NULL, foo,
                       NULL))
        return 1;
    if (pthread_join(tid1, &ret))
    {
        fprintf(stderr, "Could not join Foo Thread\n");
        return 1;
    }
    return 0;
}