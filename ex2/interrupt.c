#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

int count = 3;

static void my_sig_int()
{
    printf("you wont terminate me...\n");
    count--;
}

int main()
{

    for(;;)
    {
        if(signal(SIGINT,my_sig_int)==SIG_ERR)
        {
            perror("signal");
        }
        if(count == 0)
                exit(-1);
    }

    return 0;
}
