#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

//solution with dup isnt good because cat can recieve - in it so therefore after its done it will continue from stdin

void readBUFF(int fd)
{
    int n;
    char buff[256];
    while((n = read(fd,buff,256))>0)
        if(write(STDOUT_FILENO,buff,n) != n)
            return;
    close(fd);
}

int main (int argc, char* argv[]) 
{
    int i = 0;
    for(i = 1; i < argc; ++i)
    {
        int fd;
        if(argv[1][0]!= '-')
            fd = open(argv[i],O_RDONLY);
        else
            fd = STDIN_FILENO;
        readBUFF(fd);
    }
    if(argc < 1)
        readBUFF(STDIN_FILENO);    
};

